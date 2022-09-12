#pragma once
#include "Renderer/RenderState.h"
#include "Renderer/RenderQueue.h"

struct Model;

class IRenderer
{
public:
    virtual ~IRenderer() = default;
    virtual void Render() = 0;

    virtual void Enqueue(const RenderWorkItem& item, BlendState blend_state) {};
};

// Define in the client!
class IRenderer* CreateRenderer();
