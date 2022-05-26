cbuffer per_object_data
{
    float4x4 wvp;
};

struct PSInput
{
    float4 pos : SV_POSITION;
};

float4 Main(PSInput input) : SV_TARGET
{
    return float4(0.8f, 0.8f, 0.8f, 1.0f);
}
