#include "Renderer.h"

#if _RENDER_DEBUG
#include <dxgidebug.h>
#include <dxgi1_3.h>
#endif

#include "Core/Application.h"
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
    DX11_VERIFY(gfx::swapchain->GetDesc1(&swap_chain_desc_));

    D3D11_TEXTURE2D_DESC depth_buffer_desc = {};
    depth_buffer_desc.ArraySize = 1;
    depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_buffer_desc.CPUAccessFlags = 0; // No CPU access
    depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_buffer_desc.Width = swap_chain_desc_.Width;
    depth_buffer_desc.Height = swap_chain_desc_.Height;
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

    // Set up cbuffers
    cbuffer_per_frame_ = MakeUnique<ConstantBuffer>((uint32) sizeof(CBufferPerFrame));
    SetDebugName(cbuffer_per_frame_ ->buffer_.Get(), "Per Frame");
    cbuffer_per_view_ = MakeUnique<ConstantBuffer>((uint32)sizeof(CBufferPerView));
    SetDebugName(cbuffer_per_view_->buffer_.Get(), "Per View");
    cbuffer_light_ = MakeUnique<ConstantBuffer>((uint32)sizeof(CBufferLight));
    SetDebugName(cbuffer_light_->buffer_.Get(), "Light Data");
    cbuffer_light_view_ = MakeUnique<ConstantBuffer>((uint32)sizeof(CBufferLightView));
    SetDebugName(cbuffer_light_view_->buffer_.Get(), "Shadow Data");

    // Set up camera
    // TODO: This probably also shouldn't be in the renderer. Instead we want to grab the currently active camera from the scene
    float aspect_ratio = (float)swap_chain_desc_.Width / (float)swap_chain_desc_.Height;
    gfx::camera = Camera(Vec3(0.33f, 1.0f, 0.0f), aspect_ratio, MathUtils::DegToRad(45.0f), .1f, 100.0f);
    gfx::camera.LookAt(gfx::camera.GetPosition() + Vec3(0.0f, 0.0f, 1.0f));

    // --- Shadow Pass
    // Directional Light
    {
        D3D11_TEXTURE2D_DESC shadow_map_desc = {};
        shadow_map_desc.Width = SHADOW_MAP_SIZE;
        shadow_map_desc.Height = SHADOW_MAP_SIZE;
        shadow_map_desc.ArraySize = DirectionalLight::NUM_CASCADES;
        shadow_map_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        shadow_map_desc.Usage = D3D11_USAGE_DEFAULT;  // Read and write access by the GPU
        shadow_map_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        shadow_map_desc.CPUAccessFlags = 0; // No CPU access
        shadow_map_desc.MipLevels = 1;
        shadow_map_desc.SampleDesc.Count = 1;
        shadow_map_desc.SampleDesc.Quality = 0;

        DX11_VERIFY(gfx::device->CreateTexture2D(&shadow_map_desc, nullptr, &directional_shadow_map_));
        SetDebugName(directional_shadow_map_.Get(), "DIRECTIONAL SHADOW MAP");

        for (int i = 0; i<DirectionalLight::NUM_CASCADES; ++i)
        {
            D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
            depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
            depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            depth_stencil_view_desc.Texture2DArray.ArraySize = 1;
            depth_stencil_view_desc.Texture2DArray.FirstArraySlice = i;
            depth_stencil_view_desc.Texture2DArray.MipSlice = 0;

            DX11_VERIFY(gfx::device->CreateDepthStencilView(directional_shadow_map_.Get(), &depth_stencil_view_desc,
                &directional_shadow_map_dsvs_[i]));
            SetDebugName(directional_shadow_map_dsvs_[i].Get(), "DIRECTIONAL SHADOW MAP DSV " + i);
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC shadow_map_srv_desc = {};
        shadow_map_srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
        shadow_map_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        shadow_map_srv_desc.Texture2DArray.ArraySize = DirectionalLight::NUM_CASCADES;
        shadow_map_srv_desc.Texture2DArray.FirstArraySlice = 0;
        shadow_map_srv_desc.Texture2DArray.MipLevels = 1;

        DX11_VERIFY(gfx::device->CreateShaderResourceView(directional_shadow_map_.Get(), &shadow_map_srv_desc, &directional_shadow_map_srv_));
        SetDebugName(directional_shadow_map_srv_.Get(), "DIRECTIONAL SHADOW MAP SRV");
    }

    // Spot Light
    {
        D3D11_TEXTURE2D_DESC shadow_map_desc = {};
        shadow_map_desc.ArraySize = 1;
        shadow_map_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        shadow_map_desc.CPUAccessFlags = 0; // No CPU access
        shadow_map_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        shadow_map_desc.Width = SHADOW_MAP_SIZE;
        shadow_map_desc.Height = SHADOW_MAP_SIZE;
        shadow_map_desc.MipLevels = 1;
        shadow_map_desc.SampleDesc.Count = 1;
        shadow_map_desc.SampleDesc.Quality = 0;
        shadow_map_desc.Usage = D3D11_USAGE_DEFAULT;  // Read and write access by the GPU

        DX11_VERIFY(gfx::device->CreateTexture2D(&shadow_map_desc, nullptr, &spot_shadow_map_));
        SetDebugName(spot_shadow_map_.Get(), "SPOT SHADOW MAP");

        D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
        depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
        depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

        DX11_VERIFY(gfx::device->CreateDepthStencilView(spot_shadow_map_.Get(), &depth_stencil_view_desc, &spot_shadow_map_dsv_));
        SetDebugName(spot_shadow_map_dsv_.Get(), "SPOT SHADOW MAP DEPTH STENCIL VIEW");

        D3D11_SHADER_RESOURCE_VIEW_DESC shadow_map_srv_desc = {};
        shadow_map_srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
        shadow_map_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shadow_map_srv_desc.Texture2D.MipLevels = 1;

        DX11_VERIFY(gfx::device->CreateShaderResourceView(spot_shadow_map_.Get(), &shadow_map_srv_desc, &spot_shadow_map_srv_));
        SetDebugName(spot_shadow_map_srv_.Get(), "SPOT SHADOW MAP SRV");
    }

    // Point Light
    {
        D3D11_TEXTURE2D_DESC shadow_map_desc = {};
        shadow_map_desc.ArraySize = 6;  // cube map
        shadow_map_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        shadow_map_desc.CPUAccessFlags = 0; // No CPU access
        shadow_map_desc.Format = DXGI_FORMAT_R32_TYPELESS;
        shadow_map_desc.Width = SHADOW_MAP_SIZE;
        shadow_map_desc.Height = SHADOW_MAP_SIZE;
        shadow_map_desc.MipLevels = 1;
        shadow_map_desc.SampleDesc.Count = 1;
        shadow_map_desc.SampleDesc.Quality = 0;
        shadow_map_desc.Usage = D3D11_USAGE_DEFAULT;  // Read and write access by the GPU
        shadow_map_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

        DX11_VERIFY(gfx::device->CreateTexture2D(&shadow_map_desc, nullptr, &point_shadow_map_));
        SetDebugName(point_shadow_map_.Get(), "POINT SHADOW MAP");

        for (int i = 0; i < 6; ++i)
        {
            D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = {};
            depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
            depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            depth_stencil_view_desc.Texture2DArray.ArraySize = 1;
            depth_stencil_view_desc.Texture2DArray.FirstArraySlice = i;

            DX11_VERIFY(gfx::device->CreateDepthStencilView(point_shadow_map_.Get(), &depth_stencil_view_desc, &point_shadow_map_dsvs_[i]));
            SetDebugName(point_shadow_map_dsvs_[i].Get(), fmt::format("POINT SHADOW MAP DEPTH STENCIL VIEW {}", i).c_str());
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC shadow_map_srv_desc = {};
        shadow_map_srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
        shadow_map_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        shadow_map_srv_desc.Texture2D.MipLevels = 1;

        DX11_VERIFY(gfx::device->CreateShaderResourceView(point_shadow_map_.Get(), &shadow_map_srv_desc, &point_shadow_map_srv_));
        SetDebugName(point_shadow_map_srv_.Get(), "POINT SHADOW MAP SRV");
    }
}

Renderer::~Renderer()
{
}

void Renderer::Render()
{
    render_queue_opaque_.Sort();
    render_queue_translucent_.Sort();

    // Unbind all SRV slots - I'm just too lazy to micromanage this right now :s
    ID3D11ShaderResourceView* null_views[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    gfx::device_context->PSSetShaderResources(0, ARRAYSIZE(null_views), null_views);

    RenderShadowPass();

    D3D11_VIEWPORT viewport;
    viewport.Width = (float)swap_chain_desc_.Width;
    viewport.Height = (float)swap_chain_desc_.Height;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    gfx::device_context->RSSetViewports(1, &viewport);

    // Bind render target views to output merger stage of pipeline
    gfx ::device_context->OMSetRenderTargets(1, backbuffer_color_view_.GetAddressOf(), backbuffer_depth_view_.Get());

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

    // Update scene lighting
    light_data_ = {};
    for(const DirectionalLight& light : directional_lights_)
    {
        light_data_.directional_lights[light_data_.num_directional_lights] = light;
        ++light_data_.num_directional_lights;
    }

    for (const PointLight& light : point_lights_)
    {
        light_data_.point_lights[light_data_.num_point_lights] = light;
        ++light_data_.num_point_lights;
    }

    for (const SpotLight& light : spot_lights_)
    {
        light_data_.spot_lights[light_data_.num_spot_lights] = light;
        ++light_data_.num_spot_lights;
    }

    gfx::device_context->PSSetShaderResources(2, 1 /*num views*/, directional_shadow_map_srv_.GetAddressOf());
    gfx::device_context->PSSetShaderResources(3, 1 /*num views*/, spot_shadow_map_srv_.GetAddressOf());
    gfx::device_context->PSSetShaderResources(4, 1 /*num views*/, point_shadow_map_srv_.GetAddressOf());
    cbuffer_light_->Upload(reinterpret_cast<uint8*>(&light_data_), sizeof(CBufferLight));
    static constexpr int CBUFFER_SLOT_LIGHT_DATA = 3;
    gfx::SetConstantBuffer(cbuffer_light_->buffer_.Get(), CBUFFER_SLOT_LIGHT_DATA);

    // Forward Pass - Opaque
    {
        for (size_t idx : render_queue_opaque_.item_indices_)
        {
            const RenderWorkItem& item = render_queue_opaque_.items_[idx];
            item.mesh->model->Bind();
            item.mesh->Bind();
            item.mesh->Render();
        }
    }

    // Forward Pass - Translucent
    {
        for (size_t idx : render_queue_translucent_.item_indices_)
        {
            const RenderWorkItem& item = render_queue_translucent_.items_[idx];
            item.mesh->model->Bind();
            item.mesh->Bind();
            item.mesh->Render();
        }
    }

    render_queue_opaque_.Clear();
    render_queue_translucent_.Clear();
    directional_lights_.clear();
    point_lights_.clear();
    spot_lights_.clear();
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

void Renderer::CalculateCascades(DirectionalLight& light)
{
    static const float ratios[] = { 0.05f, 0.15f, 0.5f, 1.00f };
    for (uint32 cascade_idx = 0; cascade_idx < DirectionalLight::NUM_CASCADES; ++cascade_idx)
    {
        float ratio = ratios[cascade_idx];

        // Camera Frustum in NDC
        Vec4 frustum_corners[8] =
        {
            { -1.0f,  1.0f, 0.0f, 1.0f },   // near top left
            {  1.0f,  1.0f, 0.0f, 1.0f },   // near top right
            {  1.0f, -1.0f, 0.0f, 1.0f },   // near bottom right
            { -1.0f, -1.0f, 0.0f, 1.0f },   // near bottom right
            { -1.0f,  1.0f, 1.0f, 1.0f },   // far top left
            {  1.0f,  1.0f, 1.0f, 1.0f },   // far top right
            {  1.0f, -1.0f, 1.0f, 1.0f },   // far bottom right
            { -1.0f, -1.0f, 1.0f, 1.0f }    // far bottom left
        };

        // Calculate camera projection matrices for each cascade split
        const Mat4 cam_view = gfx::camera.GetView();
        const Mat4 cam_proj = Mat4::PerspectiveFovLH(gfx::camera.GetFov(), gfx::camera.getAspectRatio(), gfx::camera.GetNearClip(), ratio * gfx::camera.GetFarClip());
        const Mat4 cam_view_proj = cam_view * cam_proj;
        const Mat4 inv_cam_view_projection = cam_view_proj.Invert();

        // Transform corners from ndc to world space
        for (int i = 0; i < 8; ++i)
        {
            frustum_corners[i] = frustum_corners[i] * inv_cam_view_projection;
            frustum_corners[i] = frustum_corners[i] / frustum_corners[i].w;
        }

        // Build a bounding sphere around the frustum corners
        Vec4 frustum_center = { 0.0f, 0.0f, 0.0f, 0.0f };
        for (int i = 0; i < 8; ++i)
        {
            frustum_center += frustum_corners[i];
        }
        frustum_center /= 8.0f;

        float frustum_radius = 0.0f;
        for (int i = 0; i < 8; ++i)
        {
            const float radius = (frustum_corners[i] - frustum_center).Length();
            frustum_radius = std::max(frustum_radius, radius);
        }

        const float cascade_extent = frustum_radius * 2.0f;

        static const bool stabilize_cascades = true;
        if (stabilize_cascades) {
            // Stabilize by snapping to shadow map texels
            const float texels_per_world_unit = (float)SHADOW_MAP_SIZE / cascade_extent;
        
            // By scaling with texels_per_unit we can now move in texel-sized increments
            // With the inverse we can get back into our previous space
            const Mat4 to_texel_scaling = Mat4::Scaling(texels_per_world_unit, texels_per_world_unit, texels_per_world_unit);
            Mat4 to_texel = Mat4::LookAt(Vec3::ZERO, -light.direction_ws, Vec3::UP) * to_texel_scaling;
            Mat4 inv_to_texel = to_texel.Invert();

            // Move frustum center exactly onto a texel, then transform into the original space
            frustum_center = frustum_center * to_texel;
            frustum_center.x = floorf(frustum_center.x);
            frustum_center.y = floorf(frustum_center.y);
            frustum_center = frustum_center * inv_to_texel;
        }

        // Set up cascade's view project matrix
        const Vec3 shadow_cam_pos = frustum_center.xyz() - light.direction_ws * frustum_radius;
        const Mat4 shadow_view = Mat4::LookAt(shadow_cam_pos, frustum_center.xyz(), Vec3::UP);
        const Mat4 shadow_projection = Mat4::OrthographicLH(-frustum_radius, frustum_radius, -frustum_radius, frustum_radius, 0.0f, cascade_extent);
        const Mat4 shadow_view_projection = shadow_view * shadow_projection;
        light.view_projections[cascade_idx] = shadow_view_projection.Transpose();
    }
}

void Renderer::Enqueue(const DirectionalLight& light)
{
    directional_lights_.push_back(light);
    CalculateCascades(directional_lights_[directional_lights_.size() - 1]);
}

void Renderer::Enqueue(const SpotLight& light)
{
    spot_lights_.push_back(light);
}

void Renderer::Enqueue(const PointLight& light)
{
    point_lights_.push_back(light);
}

void Renderer::RenderUI()
{
}

void Renderer::RenderForwardPass()
{

}

void Renderer::RenderShadowPass()
{
    for (const DirectionalLight& light : directional_lights_)
    {
        for (uint32 cascade_idx = 0; cascade_idx < DirectionalLight::NUM_CASCADES; ++cascade_idx)
        {
            D3D11_VIEWPORT viewport;
            viewport.Width = (float) SHADOW_MAP_SIZE;
            viewport.Height = (float) SHADOW_MAP_SIZE;
            viewport.TopLeftX = 0.0f;
            viewport.TopLeftY = 0.0f;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            gfx::device_context->RSSetViewports(1, &viewport);
            gfx::SetRasterizerState(RasterizerState::Pancaking);

            gfx::device_context->OMSetRenderTargets(0, nullptr, directional_shadow_map_dsvs_[cascade_idx].Get());
            gfx::device_context->ClearDepthStencilView(directional_shadow_map_dsvs_[cascade_idx].Get(),
                D3D11_CLEAR_DEPTH, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

            light_view_data_.view_projection = light.view_projections[cascade_idx];
            cbuffer_light_view_->Upload(reinterpret_cast<uint8*>(&light_view_data_), sizeof(CBufferLightView));
            static constexpr int CBUFFER_SLOT_SHADOW_DATA = 1;
            gfx::SetConstantBuffer(cbuffer_light_view_->buffer_.Get(), CBUFFER_SLOT_SHADOW_DATA);

            static VertexShaderDesc vs_depth_desc = {
                .path = "assets/shaders/depth_map_vs.hlsl",
            };

            static Handle<VertexShader> vs_handle = gfx::resource_manager->vertex_shaders.GetHandle(vs_depth_desc);
            VertexShader* vs = gfx::resource_manager->vertex_shaders.Get(vs_handle);
            vs->Bind();
            gfx::SetPixelShader(nullptr);

            // Submit draw calls
            {
                for (size_t idx : render_queue_opaque_.item_indices_)
                {
                    const RenderWorkItem& item = render_queue_opaque_.items_[idx];
                    if (item.is_shadow_receiver)
                    {
                        item.mesh->model->Bind();
                        item.mesh->index_buffer->Bind();
                        item.mesh->pos->Bind();
                        item.mesh->Render();
                    }
                }
            }
        }
    }

    for (const SpotLight& light : spot_lights_)
    {
        D3D11_VIEWPORT viewport;
        viewport.Width = (float)SHADOW_MAP_SIZE;
        viewport.Height = (float)SHADOW_MAP_SIZE;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        gfx::device_context->RSSetViewports(1, &viewport);
        gfx::SetRasterizerState(RasterizerState::CullClockwise);    // Easy fix for shadow acne.
                                                                    // Alternative (or additionally): Add bias
                                                                    // See: http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/

        gfx::device_context->OMSetRenderTargets(0, nullptr, spot_shadow_map_dsv_.Get());
        gfx::device_context->ClearDepthStencilView(spot_shadow_map_dsv_.Get(),
            D3D11_CLEAR_DEPTH, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

        light_view_data_.view_projection = light.view_projection;
        cbuffer_light_view_->Upload(reinterpret_cast<uint8*>(&light_view_data_), sizeof(CBufferLightView));
        static constexpr int CBUFFER_SLOT_SHADOW_DATA = 1;
        gfx::SetConstantBuffer(cbuffer_light_view_->buffer_.Get(), CBUFFER_SLOT_SHADOW_DATA);

        static VertexShaderDesc vs_depth_desc = {
            .path = "assets/shaders/depth_map_vs.hlsl",
        };
        static Handle<VertexShader> vs_handle = gfx::resource_manager->vertex_shaders.GetHandle(vs_depth_desc);
        VertexShader* vs = gfx::resource_manager->vertex_shaders.Get(vs_handle);
        vs->Bind();
        gfx::SetPixelShader(nullptr);

        // Submit draw calls
        {
            for (size_t idx : render_queue_opaque_.item_indices_)
            {
                const RenderWorkItem& item = render_queue_opaque_.items_[idx];
                if(item.is_shadow_receiver)
                {
                    item.mesh->model->Bind();
                    item.mesh->index_buffer->Bind();
                    item.mesh->pos->Bind();
                    item.mesh->Render();
                }
            }
        }
    }

    for (const PointLight& light : point_lights_)
    {
        for (int i = 0; i < 6; ++i)
        {
            D3D11_VIEWPORT viewport;
            viewport.Width = (float) SHADOW_MAP_SIZE;
            viewport.Height = (float) SHADOW_MAP_SIZE;
            viewport.TopLeftX = 0.0f;
            viewport.TopLeftY = 0.0f;
            viewport.MinDepth = 0.0f;
            viewport.MaxDepth = 1.0f;
            gfx::device_context->RSSetViewports(1, &viewport);
            gfx::SetRasterizerState(RasterizerState::CullClockwise);

            gfx::device_context->OMSetRenderTargets(0, nullptr, point_shadow_map_dsvs_[i].Get());
            gfx::device_context->ClearDepthStencilView(point_shadow_map_dsvs_[i].Get(),
                D3D11_CLEAR_DEPTH, 1.0f /*depth clear val*/, 0 /*stencil clear val*/);

            light_view_data_.view_projection = light.view_projections[i];
            cbuffer_light_view_->Upload(reinterpret_cast<uint8*>(&light_view_data_), sizeof(CBufferLightView));
            static constexpr int CBUFFER_SLOT_SHADOW_DATA = 1;
            gfx::SetConstantBuffer(cbuffer_light_view_->buffer_.Get(), CBUFFER_SLOT_SHADOW_DATA);

            static VertexShaderDesc vs_depth_desc = {
                .path = "assets/shaders/depth_map_vs.hlsl",
            };

            static Handle<VertexShader> vs_handle = gfx::resource_manager->vertex_shaders.GetHandle(vs_depth_desc);
            VertexShader* vs = gfx::resource_manager->vertex_shaders.Get(vs_handle);
            vs->Bind();
            gfx::SetPixelShader(nullptr);

            // Submit draw calls
            {
                for (size_t idx : render_queue_opaque_.item_indices_)
                {
                    const RenderWorkItem& item = render_queue_opaque_.items_[idx];
                    if (item.is_shadow_receiver)
                    {
                        item.mesh->model->Bind();
                        item.mesh->index_buffer->Bind();
                        item.mesh->pos->Bind();
                        item.mesh->Render();
                    }
                }
            }
        }
    }
}

IRenderer* CreateRenderer()
{
    return new Renderer();
}
