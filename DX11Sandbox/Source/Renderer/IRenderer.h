#pragma once
#include "Renderer/RenderQueue.h"
#include "Renderer/RenderState.h"

struct Model;

class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual void Render() = 0;
    virtual void RenderUI() {};
    virtual void Enqueue(const RenderWorkItem& item, BlendState blend_state) {};

    void Present();
};

// Define in the client!
class IRenderer* CreateRenderer();
