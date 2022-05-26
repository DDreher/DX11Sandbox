#include "Renderer.h"

#include <d3dcompiler.h>

#include "Core/FileIO.h"
#include "Renderer/DX11Util.h"
#include "Renderer/Texture.h"
#include "Renderer/Vertex.h"

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
    swapchain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;  // Expected to be normalized to range 0 - 1
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

    DX11_VERIFY(D3D11CreateDeviceAndSwapChain(nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        create_device_flags,
        accepted_feature_levels, _countof(accepted_feature_levels),
        D3D11_SDK_VERSION,
        &swapchain_desc,
        &graphics_context_->swapchain,
        &graphics_context_->device,
        &device_feature_level,
        &graphics_context_->device_context));

    graphics_context_->render_state_cache = MakeShared<RenderStateCache>(graphics_context_);

    // Get render target view from swapchain backbuffer
    // Even with triple buffering we only need a single render target view
    ComPtr<ID3D11Texture2D> backbuffer;
    DX11_VERIFY(graphics_context_->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer));
    DX11_VERIFY(graphics_context_->device->CreateRenderTargetView(
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

    DX11_VERIFY(graphics_context_->device->CreateTexture2D(&depth_buffer_desc,
        nullptr,    // ptr to initial data
        &depth_buffer_));

    DX11_VERIFY(graphics_context_->device->CreateDepthStencilView(depth_buffer_.Get(), nullptr, &backbuffer_depth_view_));

    // Configure viewport, i.e. the renderable area
    viewport_.Width = static_cast<float>(window->GetWidth());
    viewport_.Height = static_cast<float>(window->GetHeight());
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    graphics_context_->device_context->RSSetViewports(1, &viewport_);

    // Bind render target views to output merger stage of pipeline
    graphics_context_->device_context->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), backbuffer_depth_view_.Get());
    
    // Scene Setup 
    mesh_.mesh_data_ = MeshData::LoadFromFile(*graphics_context_, "assets/meshes/BOSS_model_final.fbx");

    SharedPtr<Material> material_unlit_textured = MakeShared<Material>();
    material_unlit_textured->vs_ = MakeShared<Shader<EShaderType::VS>>(*graphics_context_, "assets/shaders/vs_unlit_textured.hlsl");
    material_unlit_textured->ps_ = MakeShared<Shader<EShaderType::PS>>(*graphics_context_, "assets/shaders/ps_unlit_textured.hlsl");
    material_unlit_textured->tex_ = Texture::LoadFromFile(*graphics_context_, "assets/textures/BOSS_texture_final.png");
    material_unlit_textured->is_two_sided_ = false;
    material_unlit_textured->Create(*graphics_context_);
    mesh_.material_ = material_unlit_textured;
    mesh_.transform_.scaling_ = { 1.0f };
    mesh_.transform_.translation_ = { 0.0f, -1.0f, 0.0f };

    // Set up cbuffer
    D3D11_BUFFER_DESC cbuffer_per_frame_desc = {};
    cbuffer_per_frame_desc.Usage = D3D11_USAGE_DEFAULT;   // Read / Write access
    cbuffer_per_frame_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbuffer_per_frame_desc.ByteWidth = sizeof(CBufferPerFrame);
    cbuffer_per_frame_desc.CPUAccessFlags = 0;
    DX11_VERIFY(graphics_context_->device->CreateBuffer(&cbuffer_per_frame_desc, nullptr, &cbuffer_per_frame_));

    // Set up camera
    float aspect_ratio = window->GetWidth() / static_cast<float>(window->GetHeight());
    camera_ = Camera(Vec3(0.0f, 5.0f, -10.0f), aspect_ratio, MathUtils::DegToRad(45.0f), .1f, 1000.0f);
    camera_.LookAt(Vec3(0.0f, 0.0f, 0.0f) + Vec3(0.0f, 2.0f, 0.0f));
}

void Renderer::Render()
{
    // Update objects (temporarily in here for testing purposes)
    static float dt = 1.0f / 60.0f;
    camera_.UpdateMatrices();
    mesh_.Update(dt);

    // -------------------------------------------------------------------------------
    // Clear backbuffer
    graphics_context_->device_context->ClearRenderTargetView(backbuffer_color_view_.Get(), clear_color_);
    graphics_context_->device_context->ClearDepthStencilView(backbuffer_depth_view_.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

    // -------------------------------------------------------------------------------
    // Render scene
    graphics_context_->device_context->OMSetDepthStencilState(graphics_context_->render_state_cache->GetDepthStencilState().Get(), 1);

    // Update per frame cbuffer
    Mat4 mat_vp = camera_.GetViewProjection();
    per_frame_data_.vp = mat_vp.Transpose(); // CPU: row major, GPU: col major! -> We have to transpose.
                                             // See: https://stackoverflow.com/questions/41405994/hlsl-mul-and-d3dxmatrix-order-mismatch
    graphics_context_->device_context->UpdateSubresource(cbuffer_per_frame_.Get(), 0, nullptr, &per_frame_data_, 0, 0);
    graphics_context_->device_context->VSSetConstantBuffers(0, 1, cbuffer_per_frame_.GetAddressOf());
    graphics_context_->device_context->PSSetConstantBuffers(0, 1, cbuffer_per_frame_.GetAddressOf());

    // Submit draw commands
    mesh_.Render(*graphics_context_);

    // -------------------------------------------------------------------------------
    // Swap front buffer with backbuffer
    DX11_VERIFY(graphics_context_->swapchain->Present(1, 0));
}
