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

    DX11_VERIFY(device->CreateInputLayout(VertexPosColor::LAYOUT, VertexPosColor::NUM_ELEMENTS, shader_blob->GetBufferPointer(),
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

    // 1. Specify swapchain / device requirements
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
    DX11_VERIFY(D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        create_device_flags,
        accepted_feature_levels, _countof(accepted_feature_levels),
        D3D11_SDK_VERSION,
        &swapchain_desc,
        &graphics_context_.swapchain,
        &graphics_context_.device,
        &device_feature_level,
        &graphics_context_.device_context));

    // Get render target view from swapchain backbuffer
    // Even with triple buffering we only need a single render target view (?)
    ComPtr<ID3D11Texture2D> backbuffer;
    DX11_VERIFY(graphics_context_.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer));
    DX11_VERIFY(graphics_context_.device->CreateRenderTargetView(
        backbuffer.Get(), // Ptr to render target
        nullptr,    // Ptr to D3D11_RENDER_TARGET_VIEW_DESC, nullptr to create view of entire subresource at mipmap lvl 0
        &backbuffer_color_view_));

    // Configure viewport, i.e. the renderable area
    viewport_.Width = static_cast<float>(window->GetWidth());
    viewport_.Height = static_cast<float>(window->GetHeight());
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    graphics_context_.device_context->RSSetViewports(1, &viewport_);

    // Bind render target views to output merger stage of pipeline
    graphics_context_.device_context->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), nullptr);
    
    // Scene Setup 

    // Load Shaders
    vertex_shader_.LoadFromHlsl(graphics_context_.device.Get(), "assets/shaders/vs_triangle.hlsl");
    pixel_shader_.LoadFromHlsl(graphics_context_.device.Get(), "assets/shaders/ps_triangle.hlsl");

    // Set up Vertex Buffer.
    D3D11_BUFFER_DESC vertex_buffer_desc = {};
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.ByteWidth = sizeof(VertexPosColor) * _countof(triangle_vertices_);
    vertex_buffer_desc.CPUAccessFlags = 0;

    // Fill info about the initial data of the vertex buffer
    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = triangle_vertices_;
    DX11_VERIFY(graphics_context_.device->CreateBuffer(&vertex_buffer_desc, &subresource_data, &vertex_buffer_));

    // Set up Index Buffer
    D3D11_BUFFER_DESC index_buffer_desc = {};
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.ByteWidth = sizeof(uint16) * _countof(triangle_indices_);
    index_buffer_desc.CPUAccessFlags = 0;

    subresource_data = {};
    subresource_data.pSysMem = triangle_indices_;
    DX11_VERIFY(graphics_context_.device->CreateBuffer(&index_buffer_desc, &subresource_data, &index_buffer_));
}

void Renderer::Render()
{
    // -------------------------------------------------------------------------------
    // Clear backbuffer
    graphics_context_.device_context->ClearRenderTargetView(backbuffer_color_view_.Get(), clear_color_);

    // -------------------------------------------------------------------------------
    // Render scene
    
    // Bind shaders
    graphics_context_.device_context->VSSetShader(vertex_shader_.vs.Get(), nullptr, 0);
    graphics_context_.device_context->PSSetShader(pixel_shader_.ps.Get(), nullptr, 0);

    // Bind Index and Vertex Buffers
    uint32 stride = sizeof(VertexPosColor);
    uint32 offset = 0;
    graphics_context_.device_context->IASetVertexBuffers(
        0,  // Slot we bind it to
        1,  // Num buffers
        vertex_buffer_.GetAddressOf(),    // pointer to the buffer
        &stride,    // size of each vertex
        &offset     // offset into the buffer
    );

    graphics_context_.device_context->IASetInputLayout(vertex_shader_.input_layout.Get());
    graphics_context_.device_context->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);
    graphics_context_.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    graphics_context_.device_context->DrawIndexed(_countof(triangle_indices_), 0 /*start idx*/, 0 /*idx offset*/);

    // -------------------------------------------------------------------------------
    // Swap front buffer with backbuffer
    DX11_VERIFY(graphics_context_.swapchain->Present(1, 0));
}
