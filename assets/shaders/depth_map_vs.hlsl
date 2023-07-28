
cbuffer PerFrameData : register(b0)
{
};

cbuffer LightViewData : register(b1)
{
    float4x4 mat_view;
    float4x4 mat_view_projection;
};

cbuffer PerObjectData : register(b2)
{
    float4x4 mat_world;
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
    output.pos = mul(float4(input.pos, 1.0f), mul(mat_world, mat_view_projection));
    return output;
}
