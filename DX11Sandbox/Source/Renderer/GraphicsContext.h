#pragma once
#include <d3d11.h>

#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"
#include "Renderer/RenderState.h"
#include "Renderer/ResourceManager.h"

struct GraphicsContext
{
    ComPtr<ID3D11Device> device = nullptr;                // Used for everything besides rendering :P Useful for multithreading
    ComPtr<ID3D11DeviceContext> device_context = nullptr; // Used to configure the rendering pipeline and draw geometry
    ComPtr<IDXGISwapChain> swapchain = nullptr;
    UniquePtr<RenderStateCache> render_state_cache;
    ResourceManager resource_manager;

    ~GraphicsContext()
    {
        LOG("Destroying Graphics Context");
        device_context->ClearState();
        device_context->Flush();
    }
};
