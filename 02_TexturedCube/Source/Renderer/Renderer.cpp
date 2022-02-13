#include "Renderer.h"

#include <d3dcompiler.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Core/FileIO.h"
#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"


void VertexShader::LoadFromHlsl(ID3D11Device* const device, const std::string& path)
{
    CHECK(device != nullptr);
    CHECK(vs == nullptr);
    CHECK(input_layout == nullptr);

    // Load shader source
    std::vector<char> src_data = FileIO::ReadFile(path);

    uint32_t compile_flags = 0;

    // Compile
    ComPtr<ID3DBlob> error_blob = nullptr;
    ComPtr<ID3DBlob> shader_blob = nullptr;
    HRESULT result = D3DCompile
    (
        src_data.data(),    // src to compile
        src_data.size(),    // src size
        nullptr,            // src name
        nullptr,            // defines
        nullptr,            // includes
        "Main",             // entry point
        "vs_5_0",           // combination of shader type and model, e.g. vs_5_0
        compile_flags,      // compile constants
        0,                  // effect constants. ignored => 0
        &shader_blob,       // the compiled shader
        &error_blob         // error information
    );

    if (FAILED(result) && error_blob != nullptr)
    {
        // There were some errors
        char* error_str = static_cast<char*>(error_blob->GetBufferPointer());
        LOG_ERROR("Failed to compile shader {}:\n{}", path, error_str);
        abort();
    }

    DX11_VERIFY(device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, &vs));

    DX11_VERIFY(device->CreateInputLayout(VertexPosUV::LAYOUT, VertexPosUV::NUM_ELEMENTS, shader_blob->GetBufferPointer(),
        shader_blob->GetBufferSize(), &input_layout));
}

void PixelShader::LoadFromHlsl(ID3D11Device* const device, const std::string& path)
{
    CHECK(device != nullptr);
    CHECK(ps == nullptr);

    // Load shader source
    std::vector<char> src_data = FileIO::ReadFile(path);

    uint32_t compile_flags = 0;

    // Compile
    ComPtr<ID3DBlob> error_blob = nullptr;
    ComPtr<ID3DBlob> shader_blob = nullptr;
    HRESULT result = D3DCompile
    (
        src_data.data(),    // src to compile
        src_data.size(),    // src size
        nullptr,            // src name
        nullptr,            // defines
        nullptr,            // includes
        "Main",             // entry point
        "ps_5_0",           // combination of shader type and model, e.g. vs_5_0
        compile_flags,      // compile constants
        0,                  // effect constants. ignored => 0
        &shader_blob,       // the compiled shader
        &error_blob         // error information
    );

    if (FAILED(result) && error_blob != nullptr)
    {
        // There were some errors
        char* error_str = static_cast<char*>(error_blob->GetBufferPointer());
        LOG_ERROR("Shader compilation failed: {}", error_str);
        abort();
    }

    DX11_VERIFY(device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, &ps));
}

Renderer::Renderer(Window* window)
{
    CHECK(window != nullptr);
    const HWND hwnd = static_cast<HWND>(window->GetHandle());
    CHECK(hwnd != nullptr);

    // Specify swapchain / device requirements
    DXGI_SWAP_CHAIN_DESC swapchain_desc = {};
    swapchain_desc.BufferCount = static_cast<UINT>(1);  // Num backbuffers (?) I think this does not include the front buffer
    swapchain_desc.BufferDesc.Width = static_cast<UINT>(window->GetWidth());
    swapchain_desc.BufferDesc.Height = static_cast<UINT>(window->GetHeight());
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Expected to be normalized to range 0 - 1
    //swapchain_desc.BufferDesc.RefreshRate = 0; // VSync
    swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // Describes surface usage and CPU access for backbuffer
    swapchain_desc.SampleDesc.Count = 1;
    swapchain_desc.SampleDesc.Quality = 0;
    swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;   // Discard backbuffer content after present
    swapchain_desc.OutputWindow = hwnd;
    swapchain_desc.Windowed = TRUE;

    D3D_FEATURE_LEVEL accepted_feature_levels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    D3D_FEATURE_LEVEL device_feature_level;

    uint32 create_device_flags = 0;
#if _RENDER_DEBUG
    create_device_flags = D3D11_CREATE_DEVICE_DEBUG; // Enable debug layers
#endif

    // Create swapchain and device
    DX11_VERIFY(D3D11CreateDeviceAndSwapChain(
        nullptr,    // video adapter to use. If null, use first adapter
        D3D_DRIVER_TYPE_HARDWARE,   // Hardware driver -> Best performance
        nullptr,    // Handle to dll that implements software rasterizer. Won't need this.
        create_device_flags,
        accepted_feature_levels, _countof(accepted_feature_levels),
        D3D11_SDK_VERSION,
        &swapchain_desc,
        &swapchain_,
        &device_,
        &device_feature_level,
        &device_context_));

    // Get render target view from swapchain backbuffer
    // Even with triple buffering we only need a single render target view (?)
    ComPtr<ID3D11Texture2D> backbuffer;
    DX11_VERIFY(swapchain_->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer));
    DX11_VERIFY(device_->CreateRenderTargetView(
        backbuffer.Get(), // Ptr to render target
        nullptr,    // Ptr to D3D11_RENDER_TARGET_VIEW_DESC, nullptr to create view of entire subresource at mipmap lvl 0
        &backbuffer_color_view_));

    // Create depth/stencil buffer and view
    D3D11_TEXTURE2D_DESC depth_buffer_desc = {};
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0; // No CPU access
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.Width = static_cast<UINT>(window->GetWidth());
    depth_buffer_desc.Height = static_cast<UINT>(window->GetHeight());
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.SampleDesc.Count = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;  // Read and write access by the GPU

    DX11_VERIFY(device_->CreateTexture2D(&depth_buffer_desc,
        nullptr,    // ptr to initial data
        &depth_buffer_));

    DX11_VERIFY(device_->CreateDepthStencilView(depth_buffer_.Get(), nullptr, &backbuffer_depth_view_));

    // Create depth stencil state -> configure depth test
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
    depth_stencil_desc.DepthEnable = TRUE;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
    depth_stencil_desc.StencilEnable = FALSE;
    DX11_VERIFY(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_));

    // Create rasterizer state
    D3D11_RASTERIZER_DESC rasterizer_desc = {};
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.CullMode = D3D11_CULL_BACK;
    rasterizer_desc.DepthBias = 0;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.ScissorEnable = FALSE;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    DX11_VERIFY(device_->CreateRasterizerState(&rasterizer_desc, &rs_solid_));

    rasterizer_desc = {};
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.CullMode = D3D11_CULL_NONE;
    rasterizer_desc.DepthBias = 0;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.ScissorEnable = FALSE;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    DX11_VERIFY(device_->CreateRasterizerState(&rasterizer_desc, &rs_wireframe_));

    rasterizer_desc = {};
    rasterizer_desc.AntialiasedLineEnable = FALSE;
    rasterizer_desc.CullMode = D3D11_CULL_NONE;
    rasterizer_desc.DepthBias = 0;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.DepthClipEnable = TRUE;
    rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    rasterizer_desc.FrontCounterClockwise = FALSE;
    rasterizer_desc.MultisampleEnable = FALSE;
    rasterizer_desc.ScissorEnable = FALSE;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    DX11_VERIFY(device_->CreateRasterizerState(&rasterizer_desc, &rs_double_sided_));

    // Configure viewport, i.e. the renderable area
    viewport_.Width = static_cast<float>(window->GetWidth());
    viewport_.Height = static_cast<float>(window->GetHeight());
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    device_context_->RSSetViewports(1, &viewport_);

    // Bind render target views to output merger stage of pipeline
    device_context_->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), backbuffer_depth_view_.Get());
    
    // Scene Setup 

    // Load Shaders
    vertex_shader_.LoadFromHlsl(device_.Get(), "assets/shaders/vs_unlit.hlsl");
    pixel_shader_.LoadFromHlsl(device_.Get(), "assets/shaders/ps_unlit.hlsl");

    // Set up Vertex Buffer.
    D3D11_BUFFER_DESC vertex_buffer_desc = {};
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.ByteWidth = sizeof(VertexPosUV) * _countof(cube_textured_vertices_);
    vertex_buffer_desc.CPUAccessFlags = 0;

    // Fill info about the initial data of the vertex buffer
    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = cube_textured_vertices_;
    DX11_VERIFY(device_->CreateBuffer(&vertex_buffer_desc, &subresource_data, &vertex_buffer_));

    // Set up Index Buffer
    D3D11_BUFFER_DESC index_buffer_desc = {};
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.ByteWidth = sizeof(uint16) * _countof(cube_tex_indices_);
    index_buffer_desc.CPUAccessFlags = 0;

    subresource_data = {};
    subresource_data.pSysMem = cube_tex_indices_;
    DX11_VERIFY(device_->CreateBuffer(&index_buffer_desc, &subresource_data, &index_buffer_));

    // Set up cbuffer
    D3D11_BUFFER_DESC cbuffer_per_object_desc = {};
    cbuffer_per_object_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    cbuffer_per_object_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbuffer_per_object_desc.ByteWidth = sizeof(CBufferPerObject);
    cbuffer_per_object_desc.CPUAccessFlags = 0;
    DX11_VERIFY(device_->CreateBuffer(&cbuffer_per_object_desc, nullptr, &cbuffer_per_object_));

    // Set up camera
    float aspect_ratio = window->GetWidth() / static_cast<float>(window->GetHeight());
    camera_ = Camera(Vec3(0.0f, 0.0f, -10.0f), aspect_ratio, MathUtils::DegToRad(45.0f), .1f, 1000.0f);

    // Load texture
    int tex_width;
    int tex_height;
    int tex_channels;
    std::string texture_path = "assets/textures/test_texture.png";
    stbi_uc* tex_data = stbi_load(texture_path.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
    int image_pitch = tex_width * 4 * sizeof(uint8);

    D3D11_TEXTURE2D_DESC texture_desc = {};
    texture_desc.Width = tex_width;
    texture_desc.Height = tex_height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    
    D3D11_SUBRESOURCE_DATA texture_subresource_data = {};
    texture_subresource_data.pSysMem = tex_data;
    texture_subresource_data.SysMemPitch = image_pitch;
    DX11_VERIFY(device_->CreateTexture2D(&texture_desc, &texture_subresource_data, &texture_));
    stbi_image_free(tex_data);

    DX11_VERIFY(device_->CreateShaderResourceView(texture_.Get(), nullptr, &texture_srv_));

    D3D11_SAMPLER_DESC texture_sampler_desc = {};
    texture_sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    texture_sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    texture_sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    texture_sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    texture_sampler_desc.MipLODBias = 0.0f;
    texture_sampler_desc.MaxAnisotropy = 1;
    texture_sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    texture_sampler_desc.BorderColor[0] = 0.0f;
    texture_sampler_desc.BorderColor[1] = 0.0f;
    texture_sampler_desc.BorderColor[2] = 0.0f;
    texture_sampler_desc.BorderColor[3] = 0.0f;
    texture_sampler_desc.MinLOD = -FLT_MAX;
    texture_sampler_desc.MaxLOD = FLT_MAX;
    DX11_VERIFY(device_->CreateSamplerState(&texture_sampler_desc, &texture_sampler_state_));

    D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
    render_target_blend_desc.BlendEnable = true;

    // FinalColor = SrcColor * SrcBlend [OP] DestColor * DestBlend
    render_target_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
    
    // FinalAlpha = SrcAlpha * SourceBlendFactor [OP] DestAlpha * DestBlendFactor
    render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
    render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
    render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC blend_state_desc = {};
    blend_state_desc.AlphaToCoverageEnable = false;
    blend_state_desc.RenderTarget[0] = render_target_blend_desc;
    DX11_VERIFY(device_->CreateBlendState(&blend_state_desc, &blend_state_transparent_));
}

void Renderer::Render()
{
    // Update object (temporarily in here for testing purposes)
    static float dt = 1.0f / 60.0f;
    static float angle = 0.0f;
    angle += 45.0f * dt;

    // -------------------------------------------------------------------------------
    // Clear backbuffer
    device_context_->ClearRenderTargetView(backbuffer_color_view_.Get(), clear_color_);
    device_context_->ClearDepthStencilView(backbuffer_depth_view_.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

    // -------------------------------------------------------------------------------
    // Render scene
    
    // Bind shaders
    device_context_->VSSetShader(vertex_shader_.vs.Get(), nullptr, 0);
    device_context_->PSSetShader(pixel_shader_.ps.Get(), nullptr, 0);

    // Bind Index and Vertex Buffers
    uint32 stride = sizeof(VertexPosUV);
    uint32 offset = 0;
    device_context_->IASetVertexBuffers(
        0,  // Slot we bind it to
        1,  // Num buffers
        vertex_buffer_.GetAddressOf(),    // pointer to the buffer
        &stride,    // size of each vertex
        &offset     // offset into the buffer
    );

    device_context_->OMSetDepthStencilState(depth_stencil_state_.Get(), 1);
    device_context_->IASetInputLayout(vertex_shader_.input_layout.Get());
    device_context_->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);
    device_context_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    device_context_->PSSetShaderResources(0 /*slot*/, 1 /*num views*/, texture_srv_.GetAddressOf());
    device_context_->PSSetSamplers(0, 1, texture_sampler_state_.GetAddressOf());

    // Cube
    device_context_->RSSetState(rs_double_sided_.Get());
    device_context_->OMSetBlendState(blend_state_transparent_.Get(), nullptr, 0xffffffff);

    // Update the cbuffer
    Mat4 mat_world_ = XMMatrixRotationX(XMConvertToRadians(angle)) * XMMatrixTranslation(.5f, 0.0f, 0.5f);
    Mat4 mat_wvp_ = mat_world_ * camera_.GetView() * camera_.GetProjection();
    per_object_data_.wvp = mat_wvp_.Transpose(); // CPU: row major, GPU: col major! -> We have to transpose.
    per_object_data_.alpha = 0.25f;
    device_context_->UpdateSubresource(cbuffer_per_object_.Get(), 0, nullptr, &per_object_data_, 0, 0);
    device_context_->VSSetConstantBuffers(0, 1, cbuffer_per_object_.GetAddressOf());
    device_context_->PSSetConstantBuffers(0, 1, cbuffer_per_object_.GetAddressOf());

    device_context_->DrawIndexed(_countof(cube_tex_indices_), 0 /*start idx*/, 0 /*idx offset*/);

    // -------------------------------------------------------------------------------
    // Swap front buffer with backbuffer
    DX11_VERIFY(swapchain_->Present(1, 0));
}
