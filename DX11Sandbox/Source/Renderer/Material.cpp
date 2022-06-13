#include "Material.h"

Material::Material(const MaterialDesc& desc)
{
    vs_path_ = desc.vs_path;
    ps_path_ = desc.ps_path;
    SetRasterizerState(desc.rasterizer_state);
    SetBlendState(desc.blend_state);
    SetDepthStencilState(desc.depth_stencil_state);
}

Material::~Material()
{
    LOG("Destroy material");
}

void Material::Bind(GraphicsContext& context)
{
    vs_->Bind(context);
    ps_->Bind(context);

    for(const auto& cbuffer : cbuffers_)
    {
        cbuffer->Upload();
        context.device_context->VSSetConstantBuffers(cbuffer->slot_, 1, cbuffer->buffer_.GetAddressOf());
        context.device_context->PSSetConstantBuffers(cbuffer->slot_, 1, cbuffer->buffer_.GetAddressOf());
    }

    for(const auto& [key, val] : texture_parameters_)
    {
        context.device_context->VSSetShaderResources(val.slot, 1 /*num views*/, val.tex->srv_.GetAddressOf());
        context.device_context->PSSetShaderResources(val.slot, 1 /*num views*/, val.tex->srv_.GetAddressOf());
    }

    context.device_context->OMSetBlendState(context.render_state_cache->GetBlendState(blend_state_).Get(), nullptr, 0xffffffff);
    context.device_context->OMSetDepthStencilState(context.render_state_cache->GetDepthStencilState(depth_stencil_state_).Get(), 1);
    context.device_context->RSSetState(context.render_state_cache->GetRasterizerState(rasterizer_state_).Get());
}

void Material::Create(GraphicsContext* context)
{
    CHECK(context != nullptr);
    vs_ = MakeUnique<VertexShader>(context, vs_path_);
    CHECK(vs_ != nullptr);
    ps_ = MakeUnique<PixelShader>(context, ps_path_);
    CHECK(ps_ != nullptr);

    std::unordered_set<CBufferBindingDesc> cbuffer_set;

    for (auto& desc : vs_->cbuffer_bindings_)
    {
        auto [it, was_inserted] = cbuffer_set.insert(desc);
        if(was_inserted == true)
        {
            cbuffers_.push_back(MakeUnique<ConstantBuffer>(context, desc));
        }
    }

    for (auto& desc : ps_->cbuffer_bindings_)
    {
        auto [it, was_inserted] = cbuffer_set.insert(desc);
        if (was_inserted)
        {
            cbuffers_.push_back(MakeUnique<ConstantBuffer>(context, desc));
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

void Material::SetTexture(const std::string& param_name, SharedPtr<Texture> texture)
{
    CHECK(texture != nullptr);

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
