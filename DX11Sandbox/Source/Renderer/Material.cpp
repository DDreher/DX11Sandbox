#include "Material.h"

void Material::Bind(GraphicsContext& context)
{
    if (input_layout_ != nullptr)
    {
        context.device_context->IASetInputLayout(input_layout_.Get());
    }

    if (vs_ != nullptr)
    {
        context.device_context->VSSetShader(vs_->GetNativePtr().Get(), nullptr, 0);
    }

    if (ps_ != nullptr)
    {
        context.device_context->PSSetShader(ps_->GetNativePtr().Get(), nullptr, 0);
    }

    if(tex_ != nullptr)
    {
        context.device_context->PSSetShaderResources(0 /*slot*/, 1 /*num views*/, tex_->srv_.GetAddressOf());
        context.device_context->PSSetSamplers(0, 1, context.render_state_cache->GetSamplerState(tex_->sampler_state_).GetAddressOf());
    }

    context.device_context->OMSetBlendState(context.render_state_cache->GetBlendState(blend_state_).Get(), nullptr, 0xffffffff);

    if(is_two_sided_)
    {
        context.device_context->RSSetState(context.render_state_cache->GetRasterizerState(RasterizerState::CULL_NONE).Get());
    }
    else
    {
        context.device_context->RSSetState(context.render_state_cache->GetRasterizerState(RasterizerState::CULL_CCW).Get());
    }
}

void Material::Create(const GraphicsContext& context)
{
    DX11_VERIFY(context.device->CreateInputLayout(VertexPosUVNormals::LAYOUT, VertexPosUVNormals::NUM_ELEMENTS,
        vs_->GetBlob()->GetBufferPointer(), vs_->GetBlob()->GetBufferSize(), &input_layout_));
}
