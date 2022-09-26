#include "IRenderer.h"

#include "Renderer/GraphicsContext.h"

void IRenderer::Present()
{
    // Swap front buffer with backbuffer
    DX11_VERIFY(gfx::swapchain->Present(1, 0));
}
