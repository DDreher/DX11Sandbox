#include <common.hlsli>

Texture2D tex;

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

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : UV0;
};

void Main(PSInput input, out float4 out_target_0 : SV_Target0)
{
    float4 color = tex.Sample(sampler_linear_wrap, input.uv);

#ifdef ALPHA_CUTOFF
    clip(color.a < .5f ? -1.0f : 1.0f);
#endif

    out_target_0 = color;
}
