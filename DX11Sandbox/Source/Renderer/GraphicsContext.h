#pragma once
#include <dxgi1_4.h>
#include <d3d11_3.h>

#include "Renderer/DX11Types.h"
#include "Renderer/RenderState.h"
#include "Renderer/ResourceManager.h"
#include "Renderer/Camera.h"

class IRenderer;
class RenderStateCache;
struct ResourceManager;
class Window;

namespace gfx
{
    struct PipelineState
    {
        static constexpr size_t NUM_CBUFFER_SLOTS = 8;

        D3D11_PRIMITIVE_TOPOLOGY primitive_topology = D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        BlendState blend_state = BlendState::Invalid;
        DepthStencilState depth_stencil_state = DepthStencilState::Invalid;
        RasterizerState rasterizer_state = RasterizerState::Invalid;
        ID3D11VertexShader* vs = nullptr;
        ID3D11PixelShader* ps = nullptr;
        ID3D11InputLayout* input_layout = nullptr;
        ID3D11Buffer* vs_constant_buffers[NUM_CBUFFER_SLOTS]{};
        ID3D11Buffer* ps_constant_buffers[NUM_CBUFFER_SLOTS]{};
    };

    void Init(Window* window);
    void Shutdown();
    bool IsValid();

    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology);
    void SetBlendState(BlendState state);
    void SetDepthStencilState(DepthStencilState state);
    void SetRasterizerState(RasterizerState state);
    void SetVertexShader(ID3D11VertexShader* shader);
    void SetPixelShader(ID3D11PixelShader* shader);
    void SetInputLayout(ID3D11InputLayout* input_layout);
    void SetConstantBuffer(ID3D11Buffer* buffer, int slot);

    inline ComPtr<ID3D11Device3> device = nullptr;
    inline ComPtr<ID3D11DeviceContext3> device_context = nullptr;
    inline ComPtr<IDXGISwapChain3> swapchain = nullptr;
    inline RenderStateCache* render_state_cache = nullptr;
    inline ResourceManager* resource_manager = nullptr;
    inline IRenderer* renderer = nullptr;
    inline PipelineState pipeline_state;

    // TEMP
    inline Camera camera;
}
