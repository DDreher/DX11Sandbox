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
#include "Engine/Transform.h"
#include "Renderer/Camera.h"
#include "Renderer/ConstantBufferTypes.h"
#include "Renderer/DX11Types.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/IRenderer.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

using namespace DirectX;

struct MeshData
{
    static SharedPtr<MeshData> LoadFromFile(const std::string& asset_path);

    void Bind();

    SharedPtr<IndexBuffer> index_buffer;
    SharedPtr<VertexBuffer> pos;
    SharedPtr<VertexBuffer> uv;
    SharedPtr<VertexBuffer> normals;
};

struct Mesh
{
    Mesh() = default;
    ~Mesh() = default;

    void Update(float dt);
    void Render();

    void Bind();

    float rotation_angle_ = 0.0f;
    Transform transform_;

    SharedPtr<MeshData> mesh_data_;
    SharedPtr<Material> material_;

    CBufferPerObject per_object_data_;
    ComPtr<ID3D11Buffer> cbuffer_per_object_ = nullptr;

    std::string name_;
};

class Renderer : public IRenderer
{
public:
    Renderer();
    Renderer(const Renderer&) = delete;                 // <-- No copy!
    Renderer& operator=(const Renderer&) = delete;      // <-/
    virtual ~Renderer() override;

    virtual void Render() override;
    virtual void RenderUI() override;

private:
    ComPtr<ID3D11RenderTargetView> backbuffer_color_view_ = nullptr;   // Views for "output" of the swapchain
    ComPtr<ID3D11DepthStencilView> backbuffer_depth_view_ = nullptr;

    ComPtr<ID3D11Texture2D> depth_buffer_ = nullptr;

    // Pipeline configuration
    ComPtr<ID3D11DepthStencilState> depth_stencil_state_= nullptr;

    D3D11_VIEWPORT viewport_ = { 0 };   // Defines renderable area on screen

    // Scene
    Mesh mesh_;

    // General render settings
    float clear_color_[4] = { 100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 255.0f/255.0f };

    // View data
    CBufferPerView per_view_data_;
    UniquePtr<ConstantBuffer> cbuffer_per_view_;

    ComPtr<ID3D11Texture2D> texture_ = nullptr;
    ComPtr<ID3D11ShaderResourceView> texture_srv_ = nullptr;
    ComPtr<ID3D11SamplerState> texture_sampler_state_ = nullptr;

    Vec3 tint_ = { 1.0f, 1.0f, 1.0f };
};

IRenderer* CreateRenderer();
