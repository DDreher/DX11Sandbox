#include "Renderer/Material.h"

#include "Renderer/GraphicsContext.h"

Material::Material(const MaterialDesc& desc)
{
    vs_path_ = desc.vs_path;
    ps_path_ = desc.ps_path;
    SetRasterizerState(desc.rasterizer_state);
    SetBlendState(desc.blend_state);
    SetDepthStencilState(desc.depth_stencil_state);

    Create();
}

Material::~Material()
{
    LOG("Destroy material");
}

void Material::Bind()
{
    vs_->Bind();
    ps_->Bind();

    for(const auto& cbuffer : cbuffers_)
    {
        cbuffer->Upload();
        gfx::device_context->VSSetConstantBuffers(cbuffer->slot_, 1, cbuffer->buffer_.GetAddressOf());
        gfx::device_context->PSSetConstantBuffers(cbuffer->slot_, 1, cbuffer->buffer_.GetAddressOf());
    }

    for(const auto& [key, val] : texture_parameters_)
    {
        CHECK(val.tex.IsValid());
        Texture* tex = gfx::resource_manager->Get(val.tex);
        CHECK(tex != nullptr);
        gfx::device_context->VSSetShaderResources(val.slot, 1 /*num views*/, tex->srv_.GetAddressOf());
        gfx::device_context->PSSetShaderResources(val.slot, 1 /*num views*/, tex->srv_.GetAddressOf());
    }

    gfx::device_context->OMSetBlendState(gfx::render_state_cache->GetBlendState(blend_state_).Get(), nullptr, 0xffffffff);
    gfx::device_context->OMSetDepthStencilState(gfx::render_state_cache->GetDepthStencilState(depth_stencil_state_).Get(), 1);
    gfx::device_context->RSSetState(gfx::render_state_cache->GetRasterizerState(rasterizer_state_).Get());
}

void Material::Create()
{
    vs_ = MakeUnique<VertexShader>(vs_path_);
    CHECK(vs_ != nullptr);
    ps_ = MakeUnique<PixelShader>(ps_path_);
    CHECK(ps_ != nullptr);

    std::unordered_set<CBufferBindingDesc> cbuffer_set;

    for (auto& desc : vs_->cbuffer_bindings_)
    {
        auto [it, was_inserted] = cbuffer_set.insert(desc);
        if(was_inserted == true)
        {
            cbuffers_.push_back(MakeUnique<ConstantBuffer>(desc));
            SetDebugName(cbuffers_[cbuffers_.size() - 1]->buffer_.Get(), vs_path_ + desc.name);
        }
    }

    for (auto& desc : ps_->cbuffer_bindings_)
    {
        auto [it, was_inserted] = cbuffer_set.insert(desc);
        if (was_inserted)
        {
            cbuffers_.push_back(MakeUnique<ConstantBuffer>(desc));
            SetDebugName(cbuffers_[cbuffers_.size() - 1]->buffer_.Get(), ps_path_ + desc.name);
        }
    }

    for(const auto& texture_binding : vs_->texture_bindings_)
    {
        TextureParameter tex_param =
        {
            .name = texture_binding.name,
            .slot = (uint32) texture_binding.slot
        };
        texture_parameters_[texture_binding.name] = tex_param;
    }

    for (const auto& texture_binding : ps_->texture_bindings_)
    {
        TextureParameter tex_param =
        {
            .name = texture_binding.name,
            .slot = (uint32)texture_binding.slot
        };
        texture_parameters_[texture_binding.name] = tex_param;
    }
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
