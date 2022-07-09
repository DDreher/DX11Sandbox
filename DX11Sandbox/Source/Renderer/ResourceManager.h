#pragma once
#include "Core/Handle.h"
#include "Core/Pool.h"

#include "Renderer/Texture.h"
#include "Renderer/Material.h"

class ResourceManager
{
public:
    Handle<Texture> Create(const TextureDesc& desc);
    Handle<Texture> CreateOrGet(const TextureDesc& desc);
    Texture* Get(const TextureDesc& desc) const;
    Texture* Get(Handle<Texture> handle) const;
    void Destroy(Handle<Texture> handle);

    Handle<Material> Create(const MaterialDesc& desc);
    Handle<Material> CreateOrGet(const MaterialDesc& desc);
    Material* Get(const MaterialDesc& desc) const;
    Material* Get(Handle<Material> handle) const;
    void Destroy(Handle<Material> handle);

    Handle<VertexShader> Create(const VertexShaderDesc& desc);
    Handle<VertexShader> CreateOrGet(const VertexShaderDesc& desc);
    VertexShader* Get(const VertexShaderDesc& desc) const;
    VertexShader* Get(Handle<VertexShader> handle) const;
    void Destroy(Handle<VertexShader> handle);

private:
    std::unordered_map<TextureDesc, Handle<Texture>> texture_map_;
    Pool<Texture, Texture> texture_pool_;

    std::unordered_map<MaterialDesc, Handle<Material>> material_map_;
    Pool<Material, Material> material_pool_;

    std::unordered_map<VertexShaderDesc, Handle<VertexShader>> vertex_shader_map_;
    Pool<VertexShader, VertexShader> vertex_shader_pool_;
};
