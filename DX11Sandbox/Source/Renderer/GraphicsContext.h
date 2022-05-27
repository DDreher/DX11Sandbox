#pragma once
#include <d3d11.h>

#include "DX11Types.h"
#include "DX11Util.h"

#include "RenderState.h"

struct GraphicsContext
{
    ComPtr<ID3D11Device> device = nullptr;                // Used for everything besides rendering :P Useful for multithreading
    ComPtr<ID3D11DeviceContext> device_context = nullptr; // Used to configure the rendering pipeline and draw geometry
    ComPtr<IDXGISwapChain> swapchain = nullptr;
    UniquePtr<RenderStateCache> render_state_cache;

    ~GraphicsContext()
    {
        LOG("Destroying Graphics Context");
        device_context->ClearState();
        device_context->Flush();
    }
};
