#include "Renderer.h"

#include <d3dcompiler.h>
#if _RENDER_DEBUG
#include <dxgidebug.h>
#include <dxgi1_3.h>
#endif

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_sdl.h"

#include "Core/FileIO.h"
#include "Renderer/DX11Util.h"
#include "Renderer/Texture.h"
#include "Renderer/Vertex.h"

Renderer::Renderer()
{
    // Get render target view from swapchain backbuffer
    // Even with triple buffering we only need a single render target view
    ComPtr<ID3D11Texture2D> backbuffer;
    SetDebugName(backbuffer.Get(), "BACK BUFFER");
    DX11_VERIFY(gfx::swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer));

    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc = {};
    render_target_view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    DX11_VERIFY(gfx::device->CreateRenderTargetView(
        backbuffer.Get(), // Ptr to render target
        &render_target_view_desc,    // Ptr to D3D11_RENDER_TARGET_VIEW_DESC, nullptr to create view of entire subresource at mipmap lvl 0
        &backbuffer_color_view_));

    // Create depth/stencil buffer and view
    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc;
    DX11_VERIFY(gfx::swapchain->GetDesc1(&swap_chain_desc));

    D3D11_TEXTURE2D_DESC depth_buffer_desc = {};
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0; // No CPU access
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.Width = swap_chain_desc.Width;
    depth_buffer_desc.Height = swap_chain_desc.Height;
    depth_buffer_desc.MipLevels = 1;
    depth_buffer_desc.SampleDesc.Count = 1;
    depth_buffer_desc.SampleDesc.Quality = 0;
    depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;  // Read and write access by the GPU

    DX11_VERIFY(gfx::device->CreateTexture2D(&depth_buffer_desc,
        nullptr,    // ptr to initial data
        &depth_buffer_));
    SetDebugName(depth_buffer_.Get(), "DEPTH BUFFER");

    DX11_VERIFY(gfx::device->CreateDepthStencilView(depth_buffer_.Get(), nullptr, &backbuffer_depth_view_));
    SetDebugName(backbuffer_depth_view_.Get(), "DEPTH VIEW");

    // Configure viewport, i.e. the renderable area
    viewport_.Width = (float)swap_chain_desc.Width;
    viewport_.Height = (float)swap_chain_desc.Height;
    viewport_.TopLeftX = 0.0f;
    viewport_.TopLeftY = 0.0f;
    viewport_.MinDepth = 0.0f;
    viewport_.MaxDepth = 1.0f;
    gfx::device_context->RSSetViewports(1, &viewport_);

    // Bind default global render states
    gfx::device_context->VSSetSamplers(0, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointClamp).GetAddressOf());
    gfx::device_context->PSSetSamplers(0, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointClamp).GetAddressOf());
    gfx::device_context->VSSetSamplers(1, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointWrap).GetAddressOf());
    gfx::device_context->PSSetSamplers(1, 1, gfx::render_state_cache->GetSamplerState(SamplerState::PointWrap).GetAddressOf());
    gfx::device_context->VSSetSamplers(2, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearClamp).GetAddressOf());
    gfx::device_context->PSSetSamplers(2, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearClamp).GetAddressOf());
    gfx::device_context->VSSetSamplers(3, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearWrap).GetAddressOf());
    gfx::device_context->PSSetSamplers(3, 1, gfx::render_state_cache->GetSamplerState(SamplerState::LinearWrap).GetAddressOf());

    // Set up cbuffer
    cbuffer_per_frame_ = MakeUnique<ConstantBuffer>((uint32) sizeof(CBufferPerFrame));
    SetDebugName(cbuffer_per_frame_ ->buffer_.Get(), "Per Frame");
    cbuffer_per_view_ = MakeUnique<ConstantBuffer>((uint32)sizeof(CBufferPerView));
    SetDebugName(cbuffer_per_view_->buffer_.Get(), "Per View");

    // Set up camera
    float aspect_ratio = (float)swap_chain_desc.Width / (float)swap_chain_desc.Height;
    gfx::camera = Camera(Vec3(0.33f, 1.0f, 0.0f), aspect_ratio, MathUtils::DegToRad(45.0f), .1f, 10000.0f);
    gfx::camera.LookAt(gfx::camera.GetPosition() + Vec3(0.0f, 0.0f, 1.0f));
}

Renderer::~Renderer()
{
}

void Renderer::Render()
{
    // Bind render target views to output merger stage of pipeline
    gfx::device_context->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), backbuffer_depth_view_.Get());

    // -------------------------------------------------------------------------------
    // Clear backbuffer
    gfx::device_context->ClearRenderTargetView(backbuffer_color_view_.Get(), clear_color_);
    gfx::device_context->ClearDepthStencilView(backbuffer_depth_view_.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

    // Update per-frame cbuffer
    cbuffer_per_frame_->Upload(reinterpret_cast<uint8*>(&per_frame_data_), sizeof(CBufferPerFrame));
    static constexpr int CBUFFER_SLOT_PER_FRAME = 0;
    gfx::SetConstantBuffer(cbuffer_per_frame_->buffer_.Get(), CBUFFER_SLOT_PER_FRAME);

    // Update per-view cbuffer
    per_view_data_.mat_view = gfx::camera.GetView().Transpose(); // CPU: row major, GPU: col major! -> We have to transpose
    per_view_data_.mat_inv_view = gfx::camera.GetView().Invert().Transpose();
    per_view_data_.mat_view_projection = gfx::camera.GetViewProjection().Transpose();
    per_view_data_.mat_inv_view_projection = gfx::camera.GetViewProjection().Invert().Transpose();
    per_view_data_.pos_camera_ws = Vec4(gfx::camera.GetPosition());

    cbuffer_per_view_->Upload(reinterpret_cast<uint8*>(&per_view_data_), sizeof(CBufferPerView));
    static constexpr int CBUFFER_SLOT_PER_VIEW = 1;
    gfx::SetConstantBuffer(cbuffer_per_view_->buffer_.Get(), CBUFFER_SLOT_PER_VIEW);

    // Render meshes
    render_queue_opaque_.Submit();
    render_queue_translucent_.Submit();
}

void Renderer::RenderUI()
{
    ImGui::Begin("Debug Menu");
    ImGui::Checkbox("Render opaque", &is_opaque_queue_enabled_);
    ImGui::Checkbox("Render transparent", &is_translucent_queue_enabled_);
    ImGui::End();
}

void Renderer::Enqueue(const RenderWorkItem& item, BlendState blend_state)
{
    switch (blend_state)
    {
        case BlendState::Opaque:
            if (is_opaque_queue_enabled_)
            {
                render_queue_opaque_.Add(item);
            }
            break;
        case BlendState::Additive:
        case BlendState::PremultipliedAlpha:
        case BlendState::NonPremultipliedAlpha:
            if (is_translucent_queue_enabled_)
            {
                render_queue_translucent_.Add(item);
            }
            break;
        default:
            CHECK_NO_ENTRY();
    }
}

IRenderer* CreateRenderer()
{
    return new Renderer();
}
