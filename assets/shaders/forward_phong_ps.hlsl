#include <common.hlsli>

#define MAX_DIRECTIONAL_LIGHTS 1
#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4
#define MAX_SHININESS 128

#define DIFFUSE_TEX_BIT 1
#define NORMAL_TEX_BIT 2
#define METALLIC_ROUGHNESS_TEX_BIT 4

Texture2D tex_diffuse;
Texture2D tex_normal;

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

cbuffer PerMaterialData : register(b4)
{
    float3 base_color;
    float roughness;
    float3 specular_color;
    int bound_texture_bits;
};

struct PSInput
{
    float4 pos_cs : SV_POSITION;
    float4 pos_ws : POSITION0;
    float4 normal_ws : NORMAL0;
    float4 tangent_ws : TANGENT0;
    float2 uv : UV0;
};

int HasTex(int bound_texture_bits, int bit)
{
    return bound_texture_bits & bit > 0 ? 1 : 0;
}

float CalcPhongAttenuation(float light_attenuation, float3 surface_to_light)
{
    return 1.0f / (1.0f + light_attenuation * pow(length(surface_to_light), 2.0f));
}

void Main(PSInput input, out float4 out_color : SV_Target0)
{
    float4 material_diffuse = HasTex(bound_texture_bits, DIFFUSE_TEX_BIT) ?
        tex_diffuse.Sample(sampler_anisotropic_wrap, input.uv) :
        float4(base_color, 1.0f);

#ifdef LIGHTING_ENABLED
    out_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 surface_normal_ws = normalize(input.normal_ws.xyz);      // <-- Data is interpolated between fragments -> We have to renormalize
    float3 surface_tangent_ws = normalize(input.tangent_ws.xyz);    // <-|
    float3 surface_to_cam = normalize(pos_camera_ws.xyz - input.pos_ws.xyz);
    float3 material_specular_color = specular_color;

    // CBA to convert roughness to shininess... We'll use the roughness / metalness maps when we
    // switch to PBR.
    const float shininess = 128.0f;

    {
        for (int i = 0; i < num_directional_lights; ++i)
        {
            DirectionalLight light = directional_lights[i];
            float3 surface_to_light = normalize(-light.direction_ws);

            float3 ambient = light.color * light.ambient_intensity;
            float diffuse_intensity = saturate(dot(surface_normal_ws, surface_to_light));
            float3 diffuse = saturate(light.color * diffuse_intensity);

            float3 halfway = normalize(surface_to_cam + surface_to_light);
            float specular_intensity = pow(saturate(dot(surface_normal_ws, halfway)), shininess);
            float3 specular = material_specular_color * specular_intensity;

            // Hack to fix broken specular lights
            if(diffuse_intensity <= 0.0f)
            {
                specular = 0.0f;
            }

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
            float diffuse_intensity = saturate(dot(surface_normal_ws, surface_to_light));
            float3 diffuse = saturate(light.color * diffuse_intensity);

            float3 halfway = normalize(surface_to_cam + surface_to_light);
            float3 specular = material_specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);
            
            // Hack to fix broken specular lights
            if (diffuse_intensity <= 0.0f)
            {
                specular = 0.0f;
            }

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
                float diffuse_intensity = saturate(dot(surface_normal_ws, surface_to_light));
                float3 diffuse = saturate(light.color * diffuse_intensity);

                float3 halfway = normalize(surface_to_cam + surface_to_light);
                float3 specular = material_specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);

                // Hack to fix broken specular lights
                if (diffuse_intensity <= 0.0f)
                {
                    specular = 0.0f;
                }

                out_color += float4(light.brightness * attenuation * (ambient + diffuse + specular), 1.0f);
            }
            else
            {
                out_color += float4(light.brightness * attenuation * ambient, 1.0f);
            }
        }
    }
    out_color *= material_diffuse;
#else
    out_color = material_diffuse;
#endif

#ifdef ALPHA_CUTOFF
    clip(out_color.a < .1f ? -1.0f : 1.0f);
#endif
}
