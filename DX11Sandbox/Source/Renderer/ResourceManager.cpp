#include "Renderer/ResourceManager.h"
#include "Renderer/Texture.h"
#include "Renderer/GraphicsContext.h"

Handle<Texture> ResourceManager::CreateTexture(GraphicsContext* context, const char* path)
{
    Handle<Texture> out_handle = texture_pool_.Create();
    Texture* tex = texture_pool_.Get(out_handle);
    tex->LoadFromFile(*context, path);
    return out_handle;
}

Handle<TestStruct> ResourceManager::CreateTestStruct()
{
    Handle<TestStruct> out_handle = test_pool_.Create();
    return out_handle;
}

TestStruct* ResourceManager::Get(Handle<TestStruct> handle)
{
    return test_pool_.Get(handle);
}

void ResourceManager::Destroy(Handle<TestStruct> handle)
{
    test_pool_.Destroy(handle);
}

