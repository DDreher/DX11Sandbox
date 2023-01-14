#pragma once
#include "Core/Handle.h"
#include "Core/Pool.h"

#include "Renderer/Texture.h"
#include "Renderer/Material.h"

template<typename ResourceType, typename ResourceDescriptorType>
class ResourceCache
{
public:
    Handle<ResourceType> Create(const ResourceDescriptorType& desc)
    {
        Handle<ResourceType> out_handle = resource_pool_.Create(desc);
        ResourceType* resource = resource_pool_.Get(out_handle);
        CHECK(resource != nullptr);
        descriptor_to_handle_map_[desc] = out_handle;
        return out_handle;
    }

    Handle<ResourceType> GetHandle(const ResourceDescriptorType& desc)
    {
        Handle<ResourceType> handle;

        auto it = descriptor_to_handle_map_.find(desc);
        if (it != descriptor_to_handle_map_.end())
        {
            handle = it->second;
        }

        if (!handle.IsValid())
        {
            handle = Create(desc);
        }

        return handle;
    }

    ResourceType* Get(const ResourceDescriptorType& desc) const
    {
        ResourceType* out = nullptr;

        auto it = descriptor_to_handle_map_.find(desc);
        if (it != descriptor_to_handle_map_.end())
        {
            Handle<ResourceType> handle = it->second;
            out = Get(handle);
        }

        return out;
    }

    ResourceType* Get(Handle<ResourceType> handle) const
    {
        return resource_pool_.Get(handle);
    }

    void Destroy(Handle<ResourceType> handle)
    {
        resource_pool_.Destroy(handle);
    }

private:
    std::unordered_map<ResourceDescriptorType, Handle<ResourceType>> descriptor_to_handle_map_;
    Pool<ResourceType, ResourceType> resource_pool_;
};

struct ResourceManager
{
    ResourceCache<Texture, TextureDesc> textures;
    ResourceCache<VertexShader, VertexShaderDesc> vertex_shaders;
    ResourceCache<PixelShader, PixelShaderDesc> pixel_shaders;
    ResourceCache<UncompiledShader, UncompiledShaderDesc> uncompiled_shaders;
    ResourceCache<Material, MaterialDesc> materials;
};
