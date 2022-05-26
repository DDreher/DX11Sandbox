
cbuffer per_object_data
{
    float4x4 wvp;
};

struct VSInput
{
    float3 pos : POSITION0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
};


VSOutput Main(VSInput input)
{
    VSOutput output;
    output.pos = mul(float4(input.pos, 1.0f), wvp);
    return output;
}
