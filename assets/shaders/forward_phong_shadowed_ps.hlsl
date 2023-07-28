#include <common.hlsli>
#include <lights.hlsli>

#define MAX_SHININESS 128

Texture2D tex_diffuse : register(t0);
Texture2D tex_normal : register(t1);

Texture2DArray tex_directional_shadow_map : register(t2);
Texture2D tex_spot_shadow_map : register(t3);
TextureCube tex_point_shadow_map : register(t4);

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
    float4 pos_ss : SV_POSITION;
    float4 pos_ws : POSITION0;
    float4 normal_ws : NORMAL0;
    float4 tangent_ws : TANGENT0;
    float2 uv : UV0;
    float depth_cs : DEPTHCS;
};

float CalcPhongAttenuation(float light_attenuation, float3 surface_to_light)
{
    return 1.0f / (1.0f + light_attenuation * pow(length(surface_to_light), 2.0f));
}

int CalculateCascadeIndex(float pixel_depth)
{
    const float FAR_CLIP = 100.0f;
    const float CASCADE_SPLITS[NUM_CASCADES] = { 0.05f * FAR_CLIP, 0.15f * FAR_CLIP, 0.5f * FAR_CLIP, 1.00f * FAR_CLIP };
    int cascade_idx = NUM_CASCADES - 1;
    for(int i = NUM_CASCADES - 1; i >= 0; --i)
    {
        if (pixel_depth <= CASCADE_SPLITS[i])
        {
            cascade_idx = i;
        }
    }

    return cascade_idx;
}

void Main(PSInput input, out float4 out_color : SV_Target0)
{
    float4 material_diffuse = IsBitSet(bound_texture_bits, DIFFUSE_TEX_BIT) ?
        tex_diffuse.Sample(sampler_anisotropic_wrap, input.uv) :
        float4(base_color, 1.0f);

#ifdef LIGHTING_ENABLED
    out_color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 surface_normal_ws = normalize(input.normal_ws.xyz);      // <-- Data is interpolated between fragments -> We have to renormalize
    float3 surface_tangent_ws = normalize(input.tangent_ws.xyz);    // <-|

    if (IsBitSet(bound_texture_bits, NORMAL_TEX_BIT))
    {
        float3 surface_normal_ts = (tex_normal.Sample(sampler_anisotropic_wrap, input.uv).xyz) * 2.0f - 1.0f;
        float3 surface_bitangent_ws = cross(surface_tangent_ws, surface_normal_ws);
        float3x3 mat_tbn = float3x3(surface_tangent_ws, surface_bitangent_ws, surface_normal_ws);
        surface_normal_ws = mul(surface_normal_ts, mat_tbn);
    }

    float3 surface_to_cam = normalize(pos_camera_ws.xyz - input.pos_ws.xyz);
    float3 material_specular_color = specular_color;

    // CBA to convert roughness to shininess... We'll use the roughness / metalness maps when we
    // switch to PBR.
    const float shininess = 128.0f;

    {
        for (int i = 0; i < num_directional_lights; ++i)
        {
            DirectionalLight light = directional_lights[i];
            float shadow_factor = 0.0f;
            int cascade_idx = CalculateCascadeIndex(input.depth_cs);

            float3 cascade_vis = float3(1.0f, 1.0f, 1.0f);
#ifdef CASCADE_SPLIT_DEBUG
            if (cascade_idx == 0) { cascade_vis = float3(1.0f, 0.0f, 0.0f); }
            if (cascade_idx == 1) { cascade_vis = float3(0.0f, 1.0f, 0.0f); }
            if (cascade_idx == 2) { cascade_vis = float3(0.0f, 0.0f, 1.0f); }
            if (cascade_idx == 3) { cascade_vis = float3(1.0f, 0.0f, 1.0f); }
            if (cascade_idx == 4) { cascade_vis = float3(0.0f, 1.0f, 1.0f); }
#endif

            float4 pos_light_ndc = WorldToNDC(input.pos_ws, light.view_projection[cascade_idx]);
            const float2 shadow_map_uv = NDCToUV(pos_light_ndc.xy);
            float depth_light_vs = tex_directional_shadow_map.Sample(sampler_point_clamp, 
                float3(shadow_map_uv.xy, cascade_idx)
            ).r;

#ifdef NO_PCF
            shadow_factor = pos_light_ndc.z < depth_light_vs;
#else
            // Percentage Closer Filtering (PCF)
            // SampleCmpLevelZero with LESS_EQUAL comparison sampler for hardware PCF with 1 tap (i.e. a 2x2 filter).
            shadow_factor = tex_directional_shadow_map.SampleCmpLevelZero(sampler_shadow_pcf, float3(shadow_map_uv.xy, cascade_idx), pos_light_ndc.z);

            // We could also do this manually for bigger filter sizes than 2x2
            //const float2 TEXEL_SIZE = 1.0f / 4096.0f;
            //const int FILTER_HALFSIZE = 1;  // Does not include the center texel, so actually half-size minus one :P
            //float shadow_factor = 0.0f;
            //for(int x = -FILTER_HALFSIZE; x <= FILTER_HALFSIZE; ++x)
            //{
            //    for (int y = -FILTER_HALFSIZE; y <= FILTER_HALFSIZE; ++y)
            //    {
            //        float2 texel_offset = float2(x * TEXEL_SIZE.x, y * TEXEL_SIZE.y);
            //        float depth_light_vs = tex_directional_shadow_map.Sample(sampler_point_clamp,
            //            float2(shadow_map_uv.x + texel_offset.x, shadow_map_uv.y + texel_offset.y)).r;
            //        shadow_factor += is_unclipped * WhenLessThan(pos_light_ndc.z, depth_light_vs);
            //    }
            //}
            //shadow_factor /= (FILTER_HALFSIZE * 2.0f + 1.0f) * (FILTER_HALFSIZE * 2.0f + 1.0f);
#endif
            
            const float3 surface_to_light = normalize(-light.direction_ws);

            const float3 ambient = light.color * light.ambient_intensity * cascade_vis;
            const float diffuse_intensity = saturate(dot(surface_normal_ws, surface_to_light));
            const float3 diffuse = cascade_vis * shadow_factor * saturate(light.color * diffuse_intensity);

            const float3 halfway = normalize(surface_to_cam + surface_to_light);
            const float specular_intensity = pow(saturate(dot(surface_normal_ws, halfway)), shininess);
            float3 specular = shadow_factor * material_specular_color * specular_intensity;

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
            const PointLight light = point_lights[i];

            float3 light_to_surface = input.pos_ws.xyz - light.pos_ws;
            uint face_idx = GetCubeFaceIdxFromDirection(light_to_surface);
            float4 pos_light_ndc = WorldToNDC(input.pos_ws, light.view_projection[face_idx]);

            float3 surface_to_light = normalize(light.pos_ws - input.pos_ws.xyz);

            float shadow_factor = tex_point_shadow_map.SampleCmpLevelZero(sampler_shadow_pcf, light_to_surface, pos_light_ndc.z);

            float attenuation = CalcPhongAttenuation(light.attenuation, surface_to_light);

            float3 ambient = light.ambient_intensity * light.color;
            float diffuse_intensity = saturate(dot(surface_normal_ws, surface_to_light));
            float3 diffuse = shadow_factor * saturate(light.color * diffuse_intensity);

            float3 halfway = normalize(surface_to_cam + surface_to_light);
            float3 specular = shadow_factor * material_specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);
            
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

            // Transform vertex from world space into light's clip space
            float4 pos_light_ndc = WorldToNDC(input.pos_ws, light.view_projection);

            float2 shadow_map_uv = NDCToUV(pos_light_ndc.xy); // [-1, 1] (NDC) to [0, 1]
            float is_unclipped = saturate(shadow_map_uv.xy) == shadow_map_uv ? 1.0f : 0.0f;

            // Percentage Closer Filtering (PCF)
            float shadow_factor = tex_spot_shadow_map.SampleCmpLevelZero(sampler_shadow_pcf, shadow_map_uv, pos_light_ndc.z);

            float3 ambient = light.color * light.ambient_intensity;
            float3 surface_to_light = normalize(light.pos_ws - input.pos_ws.xyz);
            float attenuation = CalcPhongAttenuation(light.attenuation, surface_to_light);

            // Only apply diffuse and specular if fragment is inside of the spotlight's cone
            // Otherwise we only apply the ambient lighting
            // Note: We compare cosines here, not angles! (dot between two unit vectors returns the cosine of the angle between them!)
            float theta = dot(-surface_to_light, light.direction_ws);
            if(theta >= light.cos_cone_cutoff)
            {
                float diffuse_intensity = saturate(dot(surface_normal_ws, surface_to_light));
                float3 diffuse = shadow_factor * saturate(light.color * diffuse_intensity);

                float3 halfway = normalize(surface_to_cam + surface_to_light);
                float3 specular = shadow_factor * material_specular_color * pow(saturate(dot(surface_normal_ws, halfway)), shininess);

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
