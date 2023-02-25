#include "RenderState.h"

#include "DX11Util.h"
#include "GraphicsContext.h"

RenderStateCache::RenderStateCache()
{
    InitCommonBlendStates();
    InitCommonRasterizerStates();
    InitCommonDepthStencilStates();
    InitCommonSamplerStates();
}

RenderStateCache::~RenderStateCache()
{
}

void RenderStateCache::InitCommonBlendStates()
{
    // BLEND_OPAQUE
    {
        ComPtr<ID3D11BlendState> blend_state = nullptr;
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
        render_target_blend_desc.BlendEnable = false;
        render_target_blend_desc.SrcBlend = D3D11_BLEND_ONE;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::Opaque] = blend_state_desc;
        DX11_VERIFY(gfx::device->CreateBlendState(&blend_state_desc, &blend_state));
        SetDebugName(blend_state.Get(), "BLEND_OPAQUE");
        blend_state_cache_[blend_state_desc] = blend_state;
    }

    // BLEND_PREMULTIPLIED_ALPHA
    {
        ComPtr<ID3D11BlendState> blend_state = nullptr;
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
        render_target_blend_desc.BlendEnable = true;
        render_target_blend_desc.SrcBlend = D3D11_BLEND_ONE;
        render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::PremultipliedAlpha] = blend_state_desc;
        DX11_VERIFY(gfx::device->CreateBlendState(&blend_state_desc, &blend_state));
        SetDebugName(blend_state.Get(), "BLEND_PREMULTIPLIED_ALPHA");
        blend_state_cache_[blend_state_desc] = blend_state;
    }

    // BLEND_NONPREMULTIPLIED_ALPHA
    {
        ComPtr<ID3D11BlendState> blend_state = nullptr;
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
        render_target_blend_desc.BlendEnable = true;
        render_target_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::NonPremultipliedAlpha] = blend_state_desc;
        DX11_VERIFY(gfx::device->CreateBlendState(&blend_state_desc, &blend_state));
        SetDebugName(blend_state.Get(), "BLEND_NONPREMULTIPLIED_ALPHA");
        blend_state_cache_[blend_state_desc] = blend_state;
    }

    // BLEND_ADDITIVE
    {
        ComPtr<ID3D11BlendState> blend_state = nullptr;
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
        render_target_blend_desc.BlendEnable = true;
        render_target_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        render_target_blend_desc.DestBlend = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ZERO;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::Additive] = blend_state_desc;
        DX11_VERIFY(gfx::device->CreateBlendState(&blend_state_desc, &blend_state));
        SetDebugName(blend_state.Get(), "BLEND_ADDITIVE");
        blend_state_cache_[blend_state_desc] = blend_state;
    }
}

void RenderStateCache::InitCommonRasterizerStates()
{
    // WIREFRAME
    {
        ComPtr<ID3D11RasterizerState> rasterizer_state;
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.SlopeScaledDepthBias = 0.0f;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        DX11_VERIFY(gfx::device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        SetDebugName(rasterizer_state.Get(), "WIREFRAME");
        common_rasterizer_state_descriptors_[RasterizerState::Wireframe] = rasterizer_desc;
    }

    // CULL_NONE
    {
        ComPtr<ID3D11RasterizerState> rasterizer_state;
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.SlopeScaledDepthBias = 0.0f;
        rasterizer_desc.CullMode = D3D11_CULL_NONE;
        DX11_VERIFY(gfx::device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        SetDebugName(rasterizer_state.Get(), "CULL_NONE");
        common_rasterizer_state_descriptors_[RasterizerState::CullNone] = rasterizer_desc;
    }

    // CULL_CW
    {
        ComPtr<ID3D11RasterizerState> rasterizer_state;
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.SlopeScaledDepthBias = 0.0f;
        rasterizer_desc.CullMode = D3D11_CULL_FRONT;
        DX11_VERIFY(gfx::device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        SetDebugName(rasterizer_state.Get(), "CULL_CW");
        common_rasterizer_state_descriptors_[RasterizerState::CullClockwise] = rasterizer_desc;
    }

    // CULL_CCW
    {
        ComPtr<ID3D11RasterizerState> rasterizer_state;
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.DepthClipEnable = TRUE;
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.SlopeScaledDepthBias = 0.0f;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;
        DX11_VERIFY(gfx::device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        SetDebugName(rasterizer_state.Get(), "CULL_CCW");
        common_rasterizer_state_descriptors_[RasterizerState::CullCounterClockwise] = rasterizer_desc;
    }

    // Pancaking
    // => Disable depth clip so we don't cull objects in front of the near plane during shadow mapping
    {
        ComPtr<ID3D11RasterizerState> rasterizer_state;
        D3D11_RASTERIZER_DESC rasterizer_desc = {};
        rasterizer_desc.AntialiasedLineEnable = FALSE;
        rasterizer_desc.DepthBias = 0;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.DepthClipEnable = FALSE;
        rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        rasterizer_desc.FrontCounterClockwise = FALSE;
        rasterizer_desc.MultisampleEnable = FALSE;
        rasterizer_desc.ScissorEnable = FALSE;
        rasterizer_desc.SlopeScaledDepthBias = 1.0f;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;
        DX11_VERIFY(gfx::device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        SetDebugName(rasterizer_state.Get(), "PANCAKING");
        common_rasterizer_state_descriptors_[RasterizerState::Pancaking] = rasterizer_desc;
    }
}

void RenderStateCache::InitCommonDepthStencilStates()
{
    ComPtr<ID3D11DepthStencilState> state;
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.DepthEnable = TRUE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS;
    desc.StencilEnable = FALSE;
    DX11_VERIFY(gfx::device->CreateDepthStencilState(&desc, &state));
    SetDebugName(state.Get(), "DEFAULT");
    common_depth_stencil_state_descriptors_[DepthStencilState::Default] = desc;
}

void RenderStateCache::InitCommonSamplerStates()
{
    // POINT_CLAMP
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::PointClamp] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "POINT_CLAMP");
        sampler_state_cache_[desc] = state;
    }

    // POINT_WRAP
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::PointWrap] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "POINT_WRAP");
        sampler_state_cache_[desc] = state;
    }

    // LINEAR_CLAMP
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::LinearClamp] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "LINEAR_CLAMP");
        sampler_state_cache_[desc] = state;
    }

    // LINEAR_WRAP
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::LinearWrap] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "LINEAR_WRAP");
        sampler_state_cache_[desc] = state;
    }

    // ANISOTROPIC_CLAMP
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::AnisotropicClamp] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "ANISOTROPIC_CLAMP");
        sampler_state_cache_[desc] = state;
    }

    // ANISOTROPIC_WRAP
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_ANISOTROPIC;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 16;
        desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::AnisotropicWrap] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "ANISOTROPIC_WRAP");
        sampler_state_cache_[desc] = state;
    }

    // SHADOW_PCF
    {
        ComPtr<ID3D11SamplerState> state;
        D3D11_SAMPLER_DESC desc = {};
        desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        desc.MipLODBias = 0.0f;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.BorderColor[0] = 0.0f;
        desc.BorderColor[1] = 0.0f;
        desc.BorderColor[2] = 0.0f;
        desc.BorderColor[3] = 0.0f;
        desc.MinLOD = -FLT_MAX;
        desc.MaxLOD = FLT_MAX;
        common_sampler_state_descriptors_[SamplerState::ShadowPCF] = desc;
        DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state));
        SetDebugName(state.Get(), "SHADOW_PCF");
        sampler_state_cache_[desc] = state;
    }
}

ComPtr<ID3D11BlendState> RenderStateCache::GetBlendState(const D3D11_BLEND_DESC& desc)
{
    auto it = blend_state_cache_.find(desc);
    if(it != blend_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11BlendState> state_ptr;
    DX11_VERIFY(gfx::device->CreateBlendState(&desc, &state_ptr));
    blend_state_cache_[desc] = state_ptr;

    return state_ptr;
}

ComPtr<ID3D11BlendState> RenderStateCache::GetBlendState(BlendState state)
{
    D3D11_BLEND_DESC desc = common_blend_state_descriptors_[state];
    return GetBlendState(desc);
}

ComPtr<ID3D11RasterizerState> RenderStateCache::GetRasterizerState(const D3D11_RASTERIZER_DESC& desc)
{
    auto it = rasterizer_state_cache_.find(desc);
    if (it != rasterizer_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11RasterizerState> state_ptr;
    DX11_VERIFY(gfx::device->CreateRasterizerState(&desc, &state_ptr));
    rasterizer_state_cache_[desc] = state_ptr;

    return state_ptr;
}

ComPtr<ID3D11RasterizerState> RenderStateCache::GetRasterizerState(RasterizerState state)
{
    D3D11_RASTERIZER_DESC desc = common_rasterizer_state_descriptors_[state];
    return GetRasterizerState(desc);
}

ComPtr<ID3D11DepthStencilState> RenderStateCache::GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc)
{
    auto it = depth_stencil_state_cache_.find(desc);
    if (it != depth_stencil_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11DepthStencilState> state_ptr;
    DX11_VERIFY(gfx::device->CreateDepthStencilState(&desc, &state_ptr));
    depth_stencil_state_cache_[desc] = state_ptr;

    return state_ptr;
}

ComPtr<ID3D11DepthStencilState> RenderStateCache::GetDepthStencilState(DepthStencilState state)
{
    D3D11_DEPTH_STENCIL_DESC desc = common_depth_stencil_state_descriptors_[state];
    return GetDepthStencilState(desc);
}

ComPtr<ID3D11SamplerState> RenderStateCache::GetSamplerState(const D3D11_SAMPLER_DESC& desc)
{
    auto it = sampler_state_cache_.find(desc);
    if (it != sampler_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11SamplerState> state_ptr;
    DX11_VERIFY(gfx::device->CreateSamplerState(&desc, &state_ptr));
    sampler_state_cache_[desc] = state_ptr;

    return state_ptr;
}

ComPtr<ID3D11SamplerState> RenderStateCache::GetSamplerState(SamplerState state)
{
    D3D11_SAMPLER_DESC desc = common_sampler_state_descriptors_[state];
    return GetSamplerState(desc);
}
