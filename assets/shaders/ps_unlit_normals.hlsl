cbuffer per_object_data
{
    float4x4 wvp;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normals : NORMALS0;
};

float4 Main(PSInput input) : SV_TARGET
{
    return float4(input.normals, 1.0f);
}
