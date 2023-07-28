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

cbuffer DirectionalLightData : register(b3)
{
    float4 light_dir_ws;
    float4 light_color;
    float4 ambient_color;
    float ambient_intensity;
};

struct PSInput
{
    float4 pos_cs : SV_POSITION;
    float4 pos_ws : POSITION0;
    float4 normal_ws : NORMAL0;
    float2 uv : UV0;
};

void Main(PSInput input, out float4 out_color : SV_Target0)
{
    out_color = tex.Sample(sampler_linear_wrap, input.uv);

    float3 surface_normal_ws = normalize(input.normal_ws).xyz;    // Normals are interpolated between fragments -> We have to renormalize
    float3 surface_to_cam = normalize(pos_camera_ws.xyz - input.pos_ws.xyz);
    float3 surface_to_light = normalize(-light_dir_ws.xyz); //normalize(light_data.pos - pos_ws);

    float3 ambient = ambient_intensity * ambient_color.xyz;
    float3 diffuse = saturate(dot(surface_normal_ws, surface_to_light)) * light_color.xyz;

    float3 reflected_light = normalize(reflect(-surface_to_light, input.normal_ws)).xyz;
    float shininess = 128.0f;
    float3 specular = pow(saturate(dot(reflected_light, surface_normal_ws)), shininess);
    out_color *= float4((ambient + diffuse + specular), 1.0f);
    //out_color = float4(specular, 1.0f);

#ifdef ALPHA_CUTOFF
    clip(out_color.a < .5f ? -1.0f : 1.0f);
#endif
}
