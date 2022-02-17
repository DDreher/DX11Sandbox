Texture2D tex;
SamplerState tex_sampler;

cbuffer per_frame_data
{
    float4x4 vp;
};

cbuffer per_object_data
{
    float4x4 w;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : UV0;
};

float4 Main(PSInput input) : SV_TARGET
{
    float4 color = tex.Sample(tex_sampler, input.uv);
    return color;
}
