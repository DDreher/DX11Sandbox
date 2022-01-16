struct VSInput
{
    float3 pos : POSITION0;
    float3 color : COLOR0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};


VSOutput Main(VSInput input)
{
    VSOutput output;
    output.pos = float4(input.pos, 1.0f);
    output.color = float4(input.color, 1.0f);
    return output;
}
