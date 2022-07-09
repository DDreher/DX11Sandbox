#include "Renderer/ResourceManager.h"

Handle<Texture> ResourceManager::Create(const TextureDesc& desc)
{
    Handle<Texture> out_handle = texture_pool_.Create();
    Texture* tex = texture_pool_.Get(out_handle);
    tex->LoadFromFile(desc.file_path);
    texture_map_[desc] = out_handle;
    return out_handle;
}

Handle<Texture> ResourceManager::CreateOrGet(const TextureDesc& desc)
{
    Handle<Texture> handle;

    auto it = texture_map_.find(desc);
    if (it != texture_map_.end())
    {
        handle = it->second;
    }

    if(!handle.IsValid())
    {
        handle = Create(desc);
    }

    return handle;
}

Texture* ResourceManager::Get(const TextureDesc& desc) const
{
    Texture* out = nullptr;

    auto it = texture_map_.find(desc);
    if (it != texture_map_.end())
    {
        Handle<Texture> handle = it->second;
        out = Get(handle);
    }

    return out;
}

Texture* ResourceManager::Get(Handle<Texture> handle) const
{
    return texture_pool_.Get(handle);
}

Handle<Material> ResourceManager::Create(const MaterialDesc& desc)
{
    Handle<Material> out_handle = material_pool_.Create(desc);
    //Material* material = material_pool_.Get(out_handle);
    material_map_[desc] = out_handle;

    return out_handle;
}

Handle<Material> ResourceManager::CreateOrGet(const MaterialDesc& desc)
{
    Handle<Material> handle;

    auto it = material_map_.find(desc);
    if (it != material_map_.end())
    {
        handle = it->second;
    }

    if (!handle.IsValid())
    {
        handle = Create(desc);
        LOG("Created material");
    }

    return handle;
}

Material* ResourceManager::Get(const MaterialDesc& desc) const
{
    Material* out = nullptr;

    auto it = material_map_.find(desc);
    if (it != material_map_.end())
    {
        Handle<Material> handle = it->second;
        out = Get(handle);
    }

    return out;
}

Material* ResourceManager::Get(Handle<Material> handle) const
{
    return material_pool_.Get(handle);
}

void ResourceManager::Destroy(Handle<Material> handle)
{
    material_pool_.Destroy(handle);
}

Handle<VertexShader> ResourceManager::Create(const VertexShaderDesc& desc)
{
    Handle<VertexShader> out_handle = vertex_shader_pool_.Create(desc);
    VertexShader* shader = vertex_shader_pool_.Get(out_handle);
    vertex_shader_map_[desc] = out_handle;
    return out_handle;
}

Handle<VertexShader> ResourceManager::CreateOrGet(const VertexShaderDesc& desc)
{
    Handle<VertexShader> handle;

    auto it = vertex_shader_map_.find(desc);
    if (it != vertex_shader_map_.end())
    {
        handle = it->second;
    }

    if (!handle.IsValid())
    {
        handle = Create(desc);
    }

    return handle;
}

VertexShader* ResourceManager::Get(const VertexShaderDesc& desc) const
{
    VertexShader* out = nullptr;

    auto it = vertex_shader_map_.find(desc);
    if (it != vertex_shader_map_.end())
    {
        Handle<VertexShader> handle = it->second;
        out = Get(handle);
    }

    return out;
}

VertexShader* ResourceManager::Get(Handle<VertexShader> handle) const
{
    return vertex_shader_pool_.Get(handle);
}

void ResourceManager::Destroy(Handle<VertexShader> handle)
{
    vertex_shader_pool_.Destroy(handle);
}