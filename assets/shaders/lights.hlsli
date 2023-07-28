#ifndef __LIGHTS_HLSLI__
#define __LIGHTS_HLSLI__

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define NUM_CASCADES 4

struct DirectionalLight
{
    float3 direction_ws;
    float ambient_intensity;
    float3 color;
    float brightness;
    float4x4 view_projection[NUM_CASCADES];
};

struct PointLight
{
    float3 pos_ws;
    float ambient_intensity;
    float3 color;
    float attenuation;
    float4x4 view_projection[6];
    float brightness;
    float padding_0;
    float padding_1;
    float padding_2;
};

struct SpotLight
{
    float3 pos_ws;
    float ambient_intensity;
    float3 color;
    float attenuation;
    float3 direction_ws;
    float cos_cone_cutoff;
    float4x4 view_projection;
    float brightness;
    float padding_0;
    float padding_1;
    float padding_2;
};

#endif // __LIGHTS_HLSLI__
