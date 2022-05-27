#include <common.hlsli>

Texture2D tex;

cbuffer PerFrameData : register(b0)
{
    float4x4 vp;
};

cbuffer PerObjectData : register(b1)
{
    float4x4 w;
};

cbuffer PerMaterialData : register(b2)
{
    float3 tint;
}

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : UV0;
};

float4 Main(PSInput input) : SV_TARGET
{
    float4 color = tex.Sample(sampler_linear_wrap, input.uv) * float4(tint, 1.0f);
    return color;
}
