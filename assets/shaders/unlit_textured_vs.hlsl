#include <common.hlsli>

cbuffer PerFrameData : register(b0)
{
};

cbuffer PerViewData : register(b1)
{
    float4x4 mat_view;
    float4x4 mat_inv_view;
    float4x4 mat_view_projection;
    float4x4 mat_inv_view_projection;
    float4 pos_camera_ws;
};

cbuffer PerObjectData : register(b2)
{
    float4x4 mat_world;
};

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
