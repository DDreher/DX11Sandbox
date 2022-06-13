#include "RenderState.h"

#include "DX11Util.h"
#include "GraphicsContext.h"

RenderStateCache::RenderStateCache(GraphicsContext* context)
    : context_(context)
{
    CHECK(context != nullptr);
    Init();
}

RenderStateCache::~RenderStateCache()
{
}

void RenderStateCache::Init()
{
    InitCommonBlendStates();
    InitCommonRasterizerStates();
    InitCommonDepthStencilStates();
    InitCommonSamplerStates();
}

void RenderStateCache::InitCommonBlendStates()
{
    CHECK(context_ != nullptr);

    // BLEND_OPAQUE
    {
        ComPtr<ID3D11BlendState> blend_state = nullptr;
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
        render_target_blend_desc.BlendEnable = false;
        render_target_blend_desc.SrcBlend = D3D11_BLEND_ONE;
        render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::BLEND_OPAQUE] = blend_state_desc;
        DX11_VERIFY(context_->device->CreateBlendState(&blend_state_desc, &blend_state));
        blend_state_cache_[blend_state_desc] = blend_state;
    }

    // BLEND_PREMULTIPLIED_ALPHA
    {
        ComPtr<ID3D11BlendState> blend_state = nullptr;
        D3D11_RENDER_TARGET_BLEND_DESC render_target_blend_desc = {};
        render_target_blend_desc.BlendEnable = true;
        render_target_blend_desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
        render_target_blend_desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOp = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::BLEND_PREMULTIPLIED_ALPHA] = blend_state_desc;
        DX11_VERIFY(context_->device->CreateBlendState(&blend_state_desc, &blend_state));
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

        common_blend_state_descriptors_[BlendState::BLEND_NONPREMULTIPLIED_ALPHA] = blend_state_desc;
        DX11_VERIFY(context_->device->CreateBlendState(&blend_state_desc, &blend_state));
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
        render_target_blend_desc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        render_target_blend_desc.DestBlendAlpha = D3D11_BLEND_ONE;
        render_target_blend_desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        render_target_blend_desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        D3D11_BLEND_DESC blend_state_desc = {};
        blend_state_desc.AlphaToCoverageEnable = false;
        blend_state_desc.RenderTarget[0] = render_target_blend_desc;

        common_blend_state_descriptors_[BlendState::BLEND_ADDITIVE] = blend_state_desc;
        DX11_VERIFY(context_->device->CreateBlendState(&blend_state_desc, &blend_state));
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
        DX11_VERIFY(context_->device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        common_rasterizer_state_descriptors_[RasterizerState::WIREFRAME] = rasterizer_desc;
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
        DX11_VERIFY(context_->device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        common_rasterizer_state_descriptors_[RasterizerState::CULL_NONE] = rasterizer_desc;

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
        DX11_VERIFY(context_->device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        common_rasterizer_state_descriptors_[RasterizerState::CULL_CW] = rasterizer_desc;
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
        DX11_VERIFY(context_->device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state));
        common_rasterizer_state_descriptors_[RasterizerState::CULL_CCW] = rasterizer_desc;
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
    DX11_VERIFY(context_->device->CreateDepthStencilState(&desc, &state));
    common_depth_stencil_state_descriptors_[DepthStencilState::DEFAULT] = desc;
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
        common_sampler_state_descriptors_[SamplerState::POINT_CLAMP] = desc;
        DX11_VERIFY(context_->device->CreateSamplerState(&desc, &state));
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
        common_sampler_state_descriptors_[SamplerState::POINT_WRAP] = desc;
        DX11_VERIFY(context_->device->CreateSamplerState(&desc, &state));
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
        common_sampler_state_descriptors_[SamplerState::LINEAR_CLAMP] = desc;
        DX11_VERIFY(context_->device->CreateSamplerState(&desc, &state));
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
        common_sampler_state_descriptors_[SamplerState::LINEAR_WRAP] = desc;
        DX11_VERIFY(context_->device->CreateSamplerState(&desc, &state));
        sampler_state_cache_[desc] = state;
    }
}

ComPtr<ID3D11BlendState> RenderStateCache::GetBlendState(const D3D11_BLEND_DESC& desc)
{
    CHECK(context_ != nullptr);
    CHECK(context_->device != nullptr);

    auto it = blend_state_cache_.find(desc);
    if(it != blend_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11BlendState> state_ptr;
    DX11_VERIFY(context_->device->CreateBlendState(&desc, &state_ptr));
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
    CHECK(context_ != nullptr);
    CHECK(context_->device != nullptr);

    auto it = rasterizer_state_cache_.find(desc);
    if (it != rasterizer_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11RasterizerState> state_ptr;
    DX11_VERIFY(context_->device->CreateRasterizerState(&desc, &state_ptr));
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
    CHECK(context_ != nullptr);
    CHECK(context_->device != nullptr);

    auto it = depth_stencil_state_cache_.find(desc);
    if (it != depth_stencil_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11DepthStencilState> state_ptr;
    DX11_VERIFY(context_->device->CreateDepthStencilState(&desc, &state_ptr));
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
    CHECK(context_ != nullptr);
    CHECK(context_->device != nullptr);

    auto it = sampler_state_cache_.find(desc);
    if (it != sampler_state_cache_.end())
    {
        return it->second;
    }

    ComPtr<ID3D11SamplerState> state_ptr;
    DX11_VERIFY(context_->device->CreateSamplerState(&desc, &state_ptr));
    sampler_state_cache_[desc] = state_ptr;

    return state_ptr;
}

ComPtr<ID3D11SamplerState> RenderStateCache::GetSamplerState(SamplerState state)
{
    D3D11_SAMPLER_DESC desc = common_sampler_state_descriptors_[state];
    return GetSamplerState(desc);
}
