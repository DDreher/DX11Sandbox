
cbuffer PerFrameData : register(b0)
{
    float4x4 vp;
};

cbuffer PerInstanceData : register(b1)
{
    float4x4 w;
};

cbuffer PerMaterialData : register(b2)
{
    float3 tint;
}
#include <common.hlsli>

struct VSInput
{
    float3 pos : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : UV0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : UV0;
};

VSOutput Main(VSInput input)
{
    VSOutput output;

    float4x4 wvp = mul(w, vp);

    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}
