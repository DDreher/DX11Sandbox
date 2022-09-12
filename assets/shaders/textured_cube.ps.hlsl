Texture2D tex;
SamplerState tex_sampler;

cbuffer per_object_data
{
    float4x4 wvp;
    float alpha;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV0;
};

float4 Main(PSInput input) : SV_TARGET
{
    float4 color = tex.Sample(tex_sampler, input.uv);
    color.a = alpha;
    return color;
}
