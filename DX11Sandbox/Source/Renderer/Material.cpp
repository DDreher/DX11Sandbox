#include "Renderer/Material.h"

#include "Renderer/GraphicsContext.h"

Material::Material(const MaterialDesc& desc)
{
    SetRasterizerState(desc.rasterizer_state);
    SetBlendState(desc.blend_state);
    SetDepthStencilState(desc.depth_stencil_state);

    std::vector<ShaderMacro> defines;
    if(desc.is_alpha_cutoff)
    {
        static constexpr const char* ALPHA_CUTOFF = "ALPHA_CUTOFF";
        static constexpr const char* ENABLE = "1";
        static constexpr const char* DISABLE = "0";

        defines.push_back({
            .name = ALPHA_CUTOFF,
            .value = ENABLE
        });
    }

    vs_ = gfx::resource_manager->vertex_shaders.GetHandle({
        .path = desc.vs_path,
        .defines = defines
        });
    CHECK(vs_.IsValid());

    ps_ = gfx::resource_manager->pixel_shaders.GetHandle({
        .path = desc.ps_path,
        .defines = defines
        });
    CHECK(ps_.IsValid());

    std::unordered_set<CBufferBindingDesc> cbuffer_set;

    VertexShader* vs = gfx::resource_manager->vertex_shaders.Get(vs_);
    for (auto& binding_desc : vs->cbuffer_bindings_)
    {
        auto [it, was_inserted] = cbuffer_set.insert(binding_desc);
        if (was_inserted == true)
        {
            cbuffers_.push_back(MakeUnique<ConstantBuffer>(binding_desc));
            SetDebugName(cbuffers_[cbuffers_.size() - 1]->buffer_.Get(), desc.vs_path + binding_desc.name);
        }
    }

    PixelShader* ps = gfx::resource_manager->pixel_shaders.Get(ps_);
    for (auto& binding_desc : ps->cbuffer_bindings_)
    {
        auto [it, was_inserted] = cbuffer_set.insert(binding_desc);
        if (was_inserted)
        {
            cbuffers_.push_back(MakeUnique<ConstantBuffer>(binding_desc));
            SetDebugName(cbuffers_[cbuffers_.size() - 1]->buffer_.Get(), desc.ps_path + binding_desc.name);
        }
    }

    for (const auto& texture_binding : vs->texture_bindings_)
    {
        TextureParameter tex_param =
        {
            .name = texture_binding.name,
            .slot = (uint32)texture_binding.slot
        };
        texture_parameters_[texture_binding.name] = tex_param;
    }

    for (const auto& texture_binding : ps->texture_bindings_)
    {
        TextureParameter tex_param =
        {
            .name = texture_binding.name,
            .slot = (uint32)texture_binding.slot
        };
        texture_parameters_[texture_binding.name] = tex_param;
    }
}

void Material::Bind()
{
    gfx::resource_manager->vertex_shaders.Get(vs_)->Bind();
    gfx::resource_manager->pixel_shaders.Get(ps_)->Bind();

    for(const auto& cbuffer : cbuffers_)
    {
        cbuffer->Upload();
        gfx::device_context->VSSetConstantBuffers(cbuffer->slot_, 1, cbuffer->buffer_.GetAddressOf());
        gfx::device_context->PSSetConstantBuffers(cbuffer->slot_, 1, cbuffer->buffer_.GetAddressOf());
    }

    for(const auto& [key, val] : texture_parameters_)
    {
        //CHECK(val.tex.IsValid());
        Texture* tex = gfx::resource_manager->textures.Get(val.tex);
        //CHECK(tex != nullptr);
        if(tex) // TODO
        {
            gfx::device_context->VSSetShaderResources(val.slot, 1 /*num views*/, tex->srv_.GetAddressOf());
            gfx::device_context->PSSetShaderResources(val.slot, 1 /*num views*/, tex->srv_.GetAddressOf());
        }
    }

    gfx::SetBlendState(blend_state_);
    gfx::SetDepthStencilState(depth_stencil_state_);
    gfx::SetRasterizerState(rasterizer_state_);
}

void Material::SetTexture(const std::string& param_name, Handle<Texture> texture)
{
    CHECK(texture.IsValid());
    auto it = texture_parameters_.find(param_name);
    if(it != texture_parameters_.end())
    {
        TextureParameter& param = it->second;
        param.tex = texture;
    }
}

void Material::SetParam(const std::string& param_name, Vec3 val)
{
    for (auto& cbuffer : cbuffers_)
    {
        cbuffer->SetVec3(param_name, val);
    }
}

void Material::SetParam(const std::string& param_name, float val)
{
    for (auto& cbuffer : cbuffers_)
    {
        cbuffer->SetVec3(param_name, val);
    }
}
