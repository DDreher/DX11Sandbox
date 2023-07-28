#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define DIFFUSE_TEX_BIT 1
#define NORMAL_TEX_BIT 2
#define METALLIC_ROUGHNESS_TEX_BIT 4

// Global Samplers

SamplerState sampler_point_clamp : register(s0);
SamplerState sampler_point_wrap : register(s1);
SamplerState sampler_linear_clamp : register(s2);
SamplerState sampler_linear_wrap : register(s3);
SamplerState sampler_anisotropic_clamp : register(s4);
SamplerState sampler_anisotropic_wrap : register(s5);
SamplerComparisonState sampler_shadow_pcf : register(s6);

// Math

float Max3(float3 v)
{
    return max(max(v.x, v.y), v.z);
}

float3 Max3Component(float3 v)
{
    const float max_val = max(max(v.x, v.y), v.z);
    return float3(v.x >= max_val ? 1.0f : 0.0f, v.y >= max_val ? 1.0f : 0.0f, v.z >= max_val ? 1.0f : 0.0f);
}

// Space Transformations

float4 WorldToNDC(float4 v, float4x4 view_projection)
{
    // World [-..., +...] to [-1, 1] (NDC)
    float4 out_v = mul(v, view_projection);
    out_v = out_v / out_v.w;
    return out_v;
}

float2 NDCToUV(float2 coords_ndc)
{
    // [-1,1] (NDC) to [0,1] (UV)
    return float2(0.5f, -0.5f) * coords_ndc + 0.5f;
}

// General Utility Functions

int IsBitSet(int bitset, int bit)
{
    return bitset & bit > 0 ? 1 : 0;
}

bool IsSaturated(float v)
{
    return saturate(v) == v;
}

bool IsSaturated(float2 v)
{
    return saturate(v.x) == v.x && saturate(v.y) == v.y;
}

bool IsSaturated(float3 v)
{
    return saturate(v.x) == v.x && saturate(v.y) == v.y && saturate(v.z) == v.z;
}

uint GetCubeFaceIdxFromDirection(float3 dir)
{
    const float3 abs_dir = abs(dir);
    float max_axis = Max3(abs_dir);

    float3 max_components = Max3Component(abs_dir);

    if (max_components.x > 0.0f)
    {
        return dir.x >= 0.0f ? 0 : 1;
    }
    else if (max_components.y > 0.0f)
    {
        return dir.y >= 0.0f ? 2 : 3;
    }
    else
    {
        return dir.z >= 0.0f ? 4 : 5;
    }
}

float WhenEqual(float x, float y)
{
    return 1.0f - abs(sign(x - y));
}

float WhenNotEqual(float x, float y)
{
    return abs(sign(x - y));
}

float WhenGreaterThan(float x, float y)
{
    return max(sign(x - y), 0.0f);
}

float WhenLessThan(float x, float y)
{
    return max(sign(y - x), 0.0f);
}

float WhenGreaterOrEqual(float x, float y)
{
    return 1.0f - WhenLessThan(x, y);
}

float WhenLessOrEqual(float x, float y)
{
    return 1.0f - WhenGreaterThan(x, y);
}

#endif // __COMMON_HLSLI__
