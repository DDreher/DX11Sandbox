#pragma once
#include "Core/Handle.h"
#include "Core/Pool.h"
#include "Renderer/Texture.h"

struct GraphicsContext;

class ResourceManager
{
public:
    Handle<Texture> CreateTexture(GraphicsContext* context, const char* path);
    Texture* Get(Handle<Texture> handle);
    void Destroy(Handle<Texture> handle);

private:
    Pool<Texture, Texture> texture_pool_;
};
