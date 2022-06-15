#include "Renderer/GraphicsContext.h"
#include "Renderer/ResourceManager.h"

Handle<Texture> ResourceManager::CreateTexture(GraphicsContext* context, const char* path)
{
    Handle<Texture> out_handle = texture_pool_.Create();
    Texture* tex = texture_pool_.Get(out_handle);
    tex->LoadFromFile(context, path);
    return out_handle;
}

Texture* ResourceManager::Get(Handle<Texture> handle)
{
    return texture_pool_.Get(handle);
}

void ResourceManager::Destroy(Handle<Texture> handle)
{
    texture_pool_.Destroy(handle);
}

