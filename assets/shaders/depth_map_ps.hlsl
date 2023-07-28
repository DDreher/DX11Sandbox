#include <common.hlsli>

Texture2D tex_diffuse : register(t0);

cbuffer PerMaterialData : register(b4)
{
    float3 base_color;
    float roughness;
    float3 specular_color;
    int bound_texture_bits;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV0;
};

void Main(PSInput input, out float4 out_color : SV_Target0)
{
    //const float4 material_diffuse = IsBitSet(bound_texture_bits, DIFFUSE_TEX_BIT) ?
    //    tex_diffuse.Sample(sampler_anisotropic_wrap, input.uv) :
    //    float4(base_color, 1.0f);

    //static const float ALPHA_DISCARD_TRESHOLD = 0.1f;
    //if (material_diffuse.a < ALPHA_DISCARD_TRESHOLD)
    //{
    //    discard;
    //}
}
