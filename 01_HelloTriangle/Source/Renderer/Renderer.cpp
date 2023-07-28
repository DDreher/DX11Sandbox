#include "Renderer.h"

#include <d3dcompiler.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "Core/FileIO.h"
#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"

#include "Renderer/GraphicsContext.h"

namespace
{
    struct Vertex
    {
        Vec3 pos;
        Vec3 color;

        bool operator==(Vertex const& other) const
        {
            return pos == other.pos && color == other.color;
        }

        static inline const D3D11_INPUT_ELEMENT_DESC LAYOUT[] =
        {
            { "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0 /*slot_idx*/, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        static inline const uint32 NUM_ELEMENTS = ARRAYSIZE(LAYOUT);
    };
}

void SimpleVertexShader::LoadFromHlsl(ID3D11Device* const device, const std::string& path)
{
    CHECK(device != nullptr);
    CHECK(vs == nullptr);
    CHECK(input_layout == nullptr);

    // Load shader source
    std::vector<uint8> src_data = FileIO::ReadFile(path);

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

    DX11_VERIFY(device->CreateInputLayout(Vertex::LAYOUT, Vertex::NUM_ELEMENTS, shader_blob->GetBufferPointer(),
        shader_blob->GetBufferSize(), &input_layout));
}

void SimplePixelShader::LoadFromHlsl(ID3D11Device* const device, const std::string& path)
{
    CHECK(device != nullptr);
    CHECK(ps == nullptr);

    // Load shader source
    std::vector<uint8> src_data = FileIO::ReadFile(path);

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

Renderer::Renderer()
{
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    DX11_VERIFY(gfx::swapchain->GetDesc1(&swap_chain_desc));

    // Get render target view from swapchain backbuffer
    // Even with triple buffering we only need a single render target view (?)

    ComPtr<ID3D11Texture2D> backbuffer;
    DX11_VERIFY(gfx::swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer));

    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
    render_target_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    DX11_VERIFY(gfx::device->CreateRenderTargetView(
        backbuffer.Get(), // Ptr to render target
        &render_target_view_desc,    // Ptr to D3D11_RENDER_TARGET_VIEW_DESC, nullptr to create view of entire subresource at mipmap lvl 0
        &backbuffer_color_view_));

    // Configure viewport, i.e. the renderable area
    viewport_.Width = (float) swap_chain_desc.Width;
    viewport_.Height = (float) swap_chain_desc.Height;
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    gfx::device_context->RSSetViewports(1, &viewport_);

    // Load Shaders
    vertex_shader_.LoadFromHlsl(gfx::device.Get(), "assets/shaders/triangle.vs.hlsl");
    pixel_shader_.LoadFromHlsl(gfx::device.Get(), "assets/shaders/triangle.ps.hlsl");

    // Set up Vertex Buffer.
    D3D11_BUFFER_DESC vertex_buffer_desc = {};
    vertex_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.ByteWidth = sizeof(Vertex) * _countof(triangle_vertices_);
    vertex_buffer_desc.CPUAccessFlags = 0;

    // Fill info about the initial data of the vertex buffer
    D3D11_SUBRESOURCE_DATA subresource_data = {};
    subresource_data.pSysMem = triangle_vertices_;
    DX11_VERIFY(gfx::device->CreateBuffer(&vertex_buffer_desc, &subresource_data, &vertex_buffer_));

    // Set up Index Buffer
    D3D11_BUFFER_DESC index_buffer_desc = {};
    index_buffer_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.ByteWidth = sizeof(uint16) * _countof(triangle_indices_);
    index_buffer_desc.CPUAccessFlags = 0;

    subresource_data = {};
    subresource_data.pSysMem = triangle_indices_;
    DX11_VERIFY(gfx::device->CreateBuffer(&index_buffer_desc, &subresource_data, &index_buffer_));
}

void Renderer::Render()
{
    // Bind render target views to output merger stage of pipeline
    gfx::device_context->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), nullptr);

    // -------------------------------------------------------------------------------
    // Clear backbuffer
    gfx::device_context->ClearRenderTargetView(backbuffer_color_view_.Get(), clear_color_);

    // -------------------------------------------------------------------------------
    // Render scene
    
    // Bind shaders
    gfx::device_context->VSSetShader(vertex_shader_.vs.Get(), nullptr, 0);
    gfx::device_context->PSSetShader(pixel_shader_.ps.Get(), nullptr, 0);

    // Bind Index and Vertex Buffers
    uint32 stride = sizeof(Vertex);
    uint32 offset = 0;
    gfx::device_context->IASetVertexBuffers(
        0,  // Slot we bind it to
        1,  // Num buffers
        vertex_buffer_.GetAddressOf(),    // pointer to the buffer
        &stride,    // size of each vertex
        &offset     // offset into the buffer
    );

    gfx::device_context->IASetInputLayout(vertex_shader_.input_layout.Get());
    gfx::device_context->IASetIndexBuffer(index_buffer_.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0);
    gfx::device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    gfx::device_context->DrawIndexed(_countof(triangle_indices_), 0 /*start idx*/, 0 /*idx offset*/);
}

IRenderer* CreateRenderer()
{
    return new Renderer();
}
