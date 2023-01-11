
cbuffer per_object_data
{
    float4x4 wvp;
    float alpha;
};

struct VSInput
{
    float3 pos : POSITION0;
    float2 uv : UV0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : UV0;
};

VSOutput Main(VSInput input)
{
    VSOutput output;
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    output.uv = input.uv;
    return output;
}
