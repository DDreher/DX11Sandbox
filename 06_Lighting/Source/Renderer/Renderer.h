#pragma once
// Link library dependencies
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "Core/Window.h"
#include "Renderer/Camera.h"
#include "Renderer/ConstantBufferTypes.h"
#include "Renderer/DX11Types.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/IRenderer.h"
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

struct Model;

using namespace DirectX;

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct DirectionalLight
{
    Vec3 direction_ws;
    float ambient_intensity;
    Vec3 color;
    float brightness;
};

struct PointLight
{
    Vec3 position_ws;
    float ambient_intensity;
    Vec3 color;
    float attenuation;
    float brightness;
    float padding_0;
    float padding_1;
    float padding_2;
};

struct SpotLight
{
    Vec3 position_ws;
    float ambient_intensity;
    Vec3 color;
    float attenuation;
    Vec3 cone_dir_ws;
    float cos_cone_angle;
    float brightness;
    float padding_0;
    float padding_1;
    float padding_2;
};

DECLSPEC_ALIGN(16)
struct CBufferLight
{
    uint32 num_directional_lights = 0;
    uint32 num_point_lights = 0;
    uint32 num_spot_lights = 0;
    float padding = 0.0f;
    DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
    PointLight point_lights[MAX_POINT_LIGHTS];
    SpotLight spot_lights[MAX_SPOT_LIGHTS];
};

class Renderer : public IRenderer
{
public:
    Renderer();
    Renderer(const Renderer&) = delete;                 // <-- No copy!
    Renderer& operator=(const Renderer&) = delete;      // <-/
    virtual ~Renderer() final;

    virtual void Render() final;

    virtual void Enqueue(const RenderWorkItem& item, BlendState blend_state) final;
    
    void Enqueue(const DirectionalLight& light)
    {
        directional_lights_.push_back(light);
    }

    void Enqueue(const PointLight& light)
    {
        point_lights_.push_back(light);
    }

    void Enqueue(const SpotLight& light)
    {
        spot_lights_.push_back(light);
    }

    void RenderUI() final;

private:
    ComPtr<ID3D11RenderTargetView> backbuffer_color_view_ = nullptr;   // Views for "output" of the swapchain
    ComPtr<ID3D11DepthStencilView> backbuffer_depth_view_ = nullptr;

    ComPtr<ID3D11Texture2D> depth_buffer_ = nullptr;

    // Pipeline configuration
    ComPtr<ID3D11DepthStencilState> depth_stencil_state_ = nullptr;

    D3D11_VIEWPORT viewport_ = { 0 };   // Defines renderable area on screen

    // Scene
    SharedPtr<Model> model_;

    // General render settings
    float clear_color_[4] = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 255.0f / 255.0f };

    CBufferPerFrame per_frame_data_;
    UniquePtr<ConstantBuffer> cbuffer_per_frame_;
    CBufferPerView per_view_data_;
    UniquePtr<ConstantBuffer> cbuffer_per_view_;
    CBufferLight light_data_;
    UniquePtr<ConstantBuffer> cbuffer_light_;

    ComPtr<ID3D11Texture2D> texture_ = nullptr;
    ComPtr<ID3D11ShaderResourceView> texture_srv_ = nullptr;
    ComPtr<ID3D11SamplerState> texture_sampler_state_ = nullptr;

    bool is_opaque_queue_enabled_ = true;
    bool is_translucent_queue_enabled_ = true;
    RenderQueue render_queue_opaque_ = RenderQueue(RenderQueueSortType::FrontToBack);
    RenderQueue render_queue_translucent_ = RenderQueue(RenderQueueSortType::BackToFront);

    std::vector<DirectionalLight> directional_lights_;
    std::vector<PointLight> point_lights_;
    std::vector<SpotLight> spot_lights_;
};

IRenderer* CreateRenderer();
