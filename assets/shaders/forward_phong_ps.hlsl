#include <common.hlsli>

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 32
#define MAX_SPOT_LIGHTS 32

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

struct DirectionalLight
{
    float3 direction_ws;
    float ambient_intensity;
    float3 color;
    float brightness;
};

struct PointLight
{
    float3 pos_ws;
    float ambient_intensity;
    float3 color;
    float attenuation;
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
    float brightness;
    float padding_0;
    float padding_1;
    float padding_2;
};

cbuffer LightingData : register(b3)
{
    int num_directional_lights;
    int num_point_lights;
    int num_spot_lights;
    float padding;
    DirectionalLight directional_lights[MAX_DIRECTIONAL_LIGHTS];
    PointLight point_lights[MAX_POINT_LIGHTS];
    SpotLight spot_lights[MAX_SPOT_LIGHTS];
};

struct PSInput
{
    float4 pos_cs : SV_POSITION;
    float4 pos_ws : POSITION0;
    float4 normal_ws : NORMAL0;
    float2 uv : UV0;
};

float CalcPhongAttenuation(float light_attenuation, float3 surface_to_light)
{
    return 1.0f / (1.0f + light_attenuation * pow(length(surface_to_light), 2.0f));
}

void Main(PSInput input, out float4 out_color : SV_Target0)
{
    out_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 surface_normal_ws = normalize(input.normal_ws).xyz;    // Normals are interpolated between fragments -> We have to renormalize
    float3 surface_to_cam = normalize(pos_camera_ws.xyz - input.pos_ws.xyz);

    // Material attributes
    float shininess = 128.0f;
    float3 specular_color = float3(1.0f, 1.0f, 1.0f);

    {
        for (int i = 0; i < num_directional_lights; ++i)
        {
            DirectionalLight light = directional_lights[i];
            float3 surface_to_light = normalize(-light.direction_ws);

            float3 ambient = light.color * light.ambient_intensity;
            float3 diffuse = light.color * saturate(dot(surface_normal_ws, surface_to_light));

            float3 halfway = normalize(surface_to_cam + surface_to_light);
            float3 specular = specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);

            out_color += float4(light.brightness * (ambient + diffuse + specular), 1.0f);
        }
    }

    {
        for (int i = 0; i < num_point_lights; ++i)
        {
            PointLight light = point_lights[i];
            float3 surface_to_light = normalize(light.pos_ws - input.pos_ws.xyz);
            float attenuation = CalcPhongAttenuation(light.attenuation, surface_to_light);

            float3 ambient = light.ambient_intensity * light.color;
            float3 diffuse = saturate(dot(surface_normal_ws, surface_to_light)) * light.color;

            float3 halfway = normalize(surface_to_cam + surface_to_light);
            float3 specular = specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);

            out_color += float4(light.brightness * attenuation * (ambient + diffuse + specular), 1.0f);
        }
    }

    {
        for (int i = 0; i < num_spot_lights; ++i)
        {
            SpotLight light = spot_lights[i];
            float3 ambient = light.color * light.ambient_intensity;
            float3 surface_to_light = normalize(light.pos_ws - input.pos_ws.xyz);
            float attenuation = CalcPhongAttenuation(light.attenuation, surface_to_light);

            // Only apply diffuse and specular if fragment is inside of the spotlight's cone
            // Otherwise we only apply the ambient lighting
            // Note: We compare cosines here, not angles! (dot between two unit vectors returns the cosine of the angle between them!)
            float theta = dot(surface_to_light, light.direction_ws);
            if(theta >= light.cos_cone_cutoff)
            {
                float3 diffuse = light.color * saturate(dot(surface_normal_ws, surface_to_light));

                float3 halfway = normalize(surface_to_cam + surface_to_light);
                float3 specular = specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);

                out_color += float4(light.brightness * attenuation * (ambient + diffuse + specular), 1.0f);
            }
            else
            {
                out_color += float4(light.brightness * attenuation * ambient, 1.0f);
            }
        }
    }

    out_color *= tex.Sample(sampler_linear_wrap, input.uv);

#ifdef ALPHA_CUTOFF
    clip(out_color.a < .1f ? -1.0f : 1.0f);
#endif
}
