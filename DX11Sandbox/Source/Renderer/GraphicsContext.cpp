#include "Renderer/GraphicsContext.h"

#include <d3d11sdklayers.h>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_sdl.h"

#include "Core/Window.h"
#include "Renderer/DX11Util.h"
#include "Renderer/IRenderer.h"

extern IRenderer* CreateRenderer();

namespace
{
    void InitDevice(Window* window)
    {
        CHECK(window != nullptr);
        const HWND hwnd = static_cast<HWND>(window->GetHandle());
        CHECK(hwnd != nullptr);

        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.Width = static_cast<UINT>(window->GetWidth());
        swapchain_desc.Height = static_cast<UINT>(window->GetHeight());
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // Not SRGB due to DXGI_SWAP_EFFECT_FLIP_DISCARD!
                                                             // See: https://walbourn.github.io/care-and-feeding-of-modern-swapchains/
        swapchain_desc.Stereo = FALSE;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.SampleDesc.Quality = 0;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // Describes surface usage and CPU access for backbuffer
        swapchain_desc.BufferCount = 2U;
        swapchain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;   // Don't use DXGI_SWAP_EFFECT_DISCARD - causes random stutters.
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE::DXGI_ALPHA_MODE_IGNORE;

        D3D_FEATURE_LEVEL accepted_feature_levels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
        };

        D3D_FEATURE_LEVEL device_feature_level;

        uint32 create_device_flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if _RENDER_DEBUG
        create_device_flags |= D3D11_CREATE_DEVICE_DEBUG; // Enable debug layers
#endif
        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> device_context;
        DX11_VERIFY(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr, create_device_flags, accepted_feature_levels,
            _countof(accepted_feature_levels), D3D11_SDK_VERSION, &device, &device_feature_level, &device_context));

        DX11_VERIFY(device.As(&gfx::device));
        DX11_VERIFY(device_context.As(&gfx::device_context));

        ComPtr<IDXGIDevice> dxgi_device;
        DX11_VERIFY(gfx::device.As(&dxgi_device));

        ComPtr<IDXGIAdapter> dxgi_adapter;
        DX11_VERIFY(dxgi_device->GetAdapter(&dxgi_adapter));

        ComPtr<IDXGIFactory4> dxgi_factory;
        DX11_VERIFY(dxgi_adapter->GetParent(__uuidof(IDXGIFactory4), &dxgi_factory));

        ComPtr<IDXGISwapChain1> dxgi_swapchain_1;
        DX11_VERIFY(dxgi_factory->CreateSwapChainForHwnd(gfx::device.Get(), hwnd, &swapchain_desc, nullptr, nullptr, &dxgi_swapchain_1));
        DX11_VERIFY(dxgi_swapchain_1.As(&gfx::swapchain));
    }

    void InitGlobalRenderStates()
    {
        std::array<ID3D11SamplerState*, 7> samplers = {
            gfx::render_state_cache->GetSamplerState(SamplerState::PointClamp).Get(),
            gfx::render_state_cache->GetSamplerState(SamplerState::PointWrap).Get(),
            gfx::render_state_cache->GetSamplerState(SamplerState::LinearClamp).Get(),
            gfx::render_state_cache->GetSamplerState(SamplerState::LinearWrap).Get(),
            gfx::render_state_cache->GetSamplerState(SamplerState::AnisotropicClamp).Get(),
            gfx::render_state_cache->GetSamplerState(SamplerState::AnisotropicWrap).Get(),
            gfx::render_state_cache->GetSamplerState(SamplerState::ShadowPCF).Get(),
        };

        gfx::device_context->VSSetSamplers(0, (UINT) samplers.size(), samplers.data());
        gfx::device_context->PSSetSamplers(0, (UINT) samplers.size(), samplers.data());
    }
}

namespace gfx
{
    void Init(Window* window)
    {
        LOG("Initializing Graphics Context");
        CHECK(IsValid() == false);

        InitDevice(window);
        render_state_cache = new RenderStateCache();
        resource_manager = new ResourceManager();
        renderer = CreateRenderer();

        InitGlobalRenderStates();
        gfx::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplSDL2_InitForD3D(window->GetSDLHandle());
        ImGui_ImplDX11_Init(device.Get(), device_context.Get());
    }

    void Shutdown()
    {
        LOG("Shutting down Graphics Context");
        CHECK(IsValid());

        device_context->ClearState();
        device_context->Flush();

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        delete renderer;
        renderer = nullptr;

        delete render_state_cache;
        render_state_cache = nullptr;

        delete resource_manager;
        resource_manager = nullptr;

        swapchain.Reset();
        device_context.Reset();

        ReportLiveObjects(device);
        device.Reset();
    }

    bool IsValid()
    {
        return device != nullptr && device_context != nullptr && swapchain != nullptr &&
            render_state_cache != nullptr && renderer != nullptr;
    }

    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology)
    {
        if (pipeline_state.primitive_topology != primitive_topology)
        {
            device_context->IASetPrimitiveTopology(primitive_topology);
            pipeline_state.primitive_topology = primitive_topology;
        }
    }

    void SetBlendState(BlendState state)
    {
        if(pipeline_state.blend_state != state)
        {
            device_context->OMSetBlendState(render_state_cache->GetBlendState(state).Get(), nullptr, 0xffffffff);
            pipeline_state.blend_state = state;
        }
    }

    void SetDepthStencilState(DepthStencilState state)
    {
        if (pipeline_state.depth_stencil_state != state)
        {
            device_context->OMSetDepthStencilState(render_state_cache->GetDepthStencilState(state).Get(), 1);
            pipeline_state.depth_stencil_state = state;
        }
    }

    void SetRasterizerState(RasterizerState state)
    {
        if (pipeline_state.rasterizer_state != state)
        {
            device_context->RSSetState(render_state_cache->GetRasterizerState(state).Get());
            pipeline_state.rasterizer_state = state;
        }
    }

    void SetVertexShader(ID3D11VertexShader* shader)
    {
        if(pipeline_state.vs != shader)
        {
            device_context->VSSetShader(shader, nullptr, 0);
            pipeline_state.vs = shader;
        }
    }

    void SetPixelShader(ID3D11PixelShader* shader)
    {
        if (pipeline_state.ps != shader)
        {
            device_context->PSSetShader(shader, nullptr, 0);
            pipeline_state.ps = shader;
        }
    }

    void SetInputLayout(ID3D11InputLayout* input_layout)
    {
        if (pipeline_state.input_layout != input_layout)
        {
            device_context->IASetInputLayout(input_layout);
            pipeline_state.input_layout = input_layout;
        }
    }

    void SetConstantBuffer(ID3D11Buffer* buffer, int slot)
    {
        if (pipeline_state.vs_constant_buffers[slot] != buffer)
        {
            device_context->VSSetConstantBuffers(slot, 1, &buffer);
            pipeline_state.vs_constant_buffers[slot] = buffer;
        }

        if (pipeline_state.ps_constant_buffers[slot] != buffer)
        {
            device_context->PSSetConstantBuffers(slot, 1, &buffer);
            pipeline_state.ps_constant_buffers[slot] = buffer;
        }
    }
}
