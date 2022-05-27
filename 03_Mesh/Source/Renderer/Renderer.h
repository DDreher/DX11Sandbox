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
#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexBuffer.h"

using namespace DirectX;

class Renderer
{
public:
    Renderer(Window* window);
    Renderer(const Renderer&) = delete;                 // <-- No copy!
    Renderer& operator=(const Renderer&) = delete;      // <-/
    ~Renderer();

    void Render();

private:
    static MeshData LoadModel(GraphicsContext const& context, std::string const& asset_path);

    UniquePtr<GraphicsContext> graphics_context_ = MakeUnique<GraphicsContext>();
    ComPtr<ID3D11RenderTargetView> backbuffer_color_view_ = nullptr;   // Views for "output" of the swapchain
    ComPtr<ID3D11DepthStencilView> backbuffer_depth_view_ = nullptr;

    ComPtr<ID3D11Texture2D> depth_buffer_ = nullptr;

    // Pipeline configuration
    ComPtr<ID3D11DepthStencilState> depth_stencil_state_= nullptr;

    D3D11_VIEWPORT viewport_ = { 0 };   // Defines renderable area on screen

    // Scene
    Mesh mesh_;

    // Shader Resources
    UniquePtr<ConstantBuffer> cbuffer_per_frame_;

    // General render settings
    float clear_color_[4] = { 100.0f/255.0f, 149.0f/255.0f, 237.0f/255.0f, 255.0f/255.0f };

    // Object data
    CBufferPerFrame per_frame_data_;

    ComPtr<ID3D11Texture2D> texture_ = nullptr;
    ComPtr<ID3D11ShaderResourceView> texture_srv_ = nullptr;
    ComPtr<ID3D11SamplerState> texture_sampler_state_ = nullptr;

    Camera camera_;
};
