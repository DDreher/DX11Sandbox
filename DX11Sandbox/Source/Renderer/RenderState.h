#pragma once
#include <d3d11.h>
#include "DX11Types.h"

enum class BlendState : uint8
{
    Opaque,
    PremultipliedAlpha,
    NonPremultipliedAlpha,
    Additive,
    Invalid
};

MAKE_HASHABLE(D3D11_BLEND_DESC, t.AlphaToCoverageEnable, t.IndependentBlendEnable);

inline bool operator==(const D3D11_BLEND_DESC& lhs, const D3D11_BLEND_DESC& rhs)
{
    bool result = true;

    result = result &&
        lhs.AlphaToCoverageEnable == rhs.AlphaToCoverageEnable &&
        lhs.IndependentBlendEnable == rhs.IndependentBlendEnable;

    for (size_t i = 0; i < 8; i++)
    {
        result = result &&
            lhs.RenderTarget[i].BlendEnable == rhs.RenderTarget[i].BlendEnable &&
            lhs.RenderTarget[i].BlendOp == rhs.RenderTarget[i].BlendOp &&
            lhs.RenderTarget[i].BlendOpAlpha == rhs.RenderTarget[i].BlendOpAlpha &&
            lhs.RenderTarget[i].DestBlend == rhs.RenderTarget[i].DestBlend &&
            lhs.RenderTarget[i].DestBlendAlpha == rhs.RenderTarget[i].DestBlendAlpha &&
            lhs.RenderTarget[i].RenderTargetWriteMask == rhs.RenderTarget[i].RenderTargetWriteMask &&
            lhs.RenderTarget[i].SrcBlend == rhs.RenderTarget[i].SrcBlend &&
            lhs.RenderTarget[i].SrcBlendAlpha == rhs.RenderTarget[i].SrcBlendAlpha;

        if(result == false)
        {
            break;
        }
    }

    return result;
}

enum class RasterizerState : uint8
{
    Wireframe = 0,
    CullNone,
    CullClockwise,
    CullCounterClockwise,
    Pancaking,
    Invalid
};

MAKE_HASHABLE(D3D11_RASTERIZER_DESC);

inline bool operator==(const D3D11_RASTERIZER_DESC& lhs, const D3D11_RASTERIZER_DESC& rhs)
{
    bool result = lhs.AntialiasedLineEnable == rhs.AntialiasedLineEnable
        && lhs.DepthBias == rhs.DepthBias
        && lhs.DepthBiasClamp == rhs.DepthBiasClamp
        && lhs.DepthClipEnable == rhs.DepthClipEnable
        && lhs.FillMode == rhs.FillMode
        && lhs.FrontCounterClockwise == rhs.FrontCounterClockwise
        && lhs.MultisampleEnable == rhs.MultisampleEnable
        && lhs.ScissorEnable == rhs.ScissorEnable
        && lhs.SlopeScaledDepthBias == rhs.SlopeScaledDepthBias
        && lhs.CullMode == rhs.CullMode;
    return result;
}

enum class SamplerState : uint8
{
    PointClamp = 0,
    PointWrap,
    LinearClamp,
    LinearWrap,
    AnisotropicClamp,
    AnisotropicWrap,
    ShadowPCF
};
MAKE_HASHABLE(D3D11_SAMPLER_DESC);

inline bool operator==(const D3D11_SAMPLER_DESC& lhs, const D3D11_SAMPLER_DESC& rhs)
{
    bool result = lhs.Filter == rhs.Filter
        && lhs.AddressU == rhs.AddressU
        && lhs.AddressV == rhs.AddressV
        && lhs.AddressW == rhs.AddressW
        && lhs.MipLODBias == rhs.MipLODBias
        && lhs.MaxAnisotropy == rhs.MaxAnisotropy
        && lhs.ComparisonFunc == rhs.ComparisonFunc
        && lhs.BorderColor[0] == rhs.BorderColor[0]
        && lhs.BorderColor[1] == rhs.BorderColor[1]
        && lhs.BorderColor[2] == rhs.BorderColor[2]
        && lhs.BorderColor[3] == rhs.BorderColor[3]
        && lhs.MinLOD == rhs.MinLOD
        && lhs.MaxLOD == rhs.MaxLOD;

    return result;
}

enum class DepthStencilState : uint8
{
    Default,
    Invalid
};
MAKE_HASHABLE(D3D11_DEPTH_STENCIL_DESC);

inline bool operator==(const D3D11_DEPTH_STENCIL_DESC& lhs, const D3D11_DEPTH_STENCIL_DESC& rhs)
{
    bool result = memcmp(&lhs, &rhs, sizeof(D3D11_DEPTH_STENCIL_DESC)) == 0;
    return result;
}

//////////////////////////////////////////////////////////////////////////

class RenderStateCache
{
public:
    RenderStateCache();
    ~RenderStateCache();

    ComPtr<ID3D11BlendState> GetBlendState(const D3D11_BLEND_DESC& desc);
    ComPtr<ID3D11BlendState> GetBlendState(BlendState state);
    ComPtr<ID3D11RasterizerState> GetRasterizerState(const D3D11_RASTERIZER_DESC& desc);
    ComPtr<ID3D11RasterizerState> GetRasterizerState(RasterizerState state);
    ComPtr<ID3D11DepthStencilState> GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc);
    ComPtr<ID3D11DepthStencilState> GetDepthStencilState(DepthStencilState state);
    ComPtr<ID3D11SamplerState> GetSamplerState(const D3D11_SAMPLER_DESC& desc);
    ComPtr<ID3D11SamplerState> GetSamplerState(SamplerState state);

private:
    void Init();
    void InitCommonBlendStates();
    void InitCommonRasterizerStates();
    void InitCommonDepthStencilStates();
    void InitCommonSamplerStates();

    std::unordered_map<BlendState, D3D11_BLEND_DESC> common_blend_state_descriptors_;
    std::unordered_map<D3D11_BLEND_DESC, ComPtr<ID3D11BlendState>> blend_state_cache_;

    std::unordered_map<RasterizerState, D3D11_RASTERIZER_DESC> common_rasterizer_state_descriptors_;
    std::unordered_map<D3D11_RASTERIZER_DESC, ComPtr<ID3D11RasterizerState>> rasterizer_state_cache_;

    std::unordered_map<DepthStencilState, D3D11_DEPTH_STENCIL_DESC> common_depth_stencil_state_descriptors_;
    std::unordered_map<D3D11_DEPTH_STENCIL_DESC, ComPtr<ID3D11DepthStencilState>> depth_stencil_state_cache_;

    std::unordered_map<SamplerState, D3D11_SAMPLER_DESC> common_sampler_state_descriptors_;
    std::unordered_map<D3D11_SAMPLER_DESC, ComPtr<ID3D11SamplerState>> sampler_state_cache_;
};
