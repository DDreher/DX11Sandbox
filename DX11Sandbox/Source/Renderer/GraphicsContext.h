#pragma once
#include <dxgi1_4.h>
#include <d3d11_3.h>

#include "Renderer/DX11Types.h"
#include "Renderer/DX11Util.h"
#include "Renderer/RenderState.h"
#include "Renderer/ResourceManager.h"

class IRenderer;
class RenderStateCache;
class ResourceManager;
class Window;

namespace gfx
{
    void Init(Window* window);
    void Shutdown();
    bool IsValid();

    extern ComPtr<ID3D11Device3> device;
    extern ComPtr<ID3D11DeviceContext3> device_context;
    extern ComPtr<IDXGISwapChain3> swapchain;
    extern RenderStateCache* render_state_cache;
    extern ResourceManager* resource_manager;
    extern IRenderer* renderer;
}
