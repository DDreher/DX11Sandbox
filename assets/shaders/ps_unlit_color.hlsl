cbuffer per_frame_data : register(b0)
{
    float4x4 vp;
};

cbuffer per_object_data : register(b1)
{
    float4x4 w;
    float3 base_color;
    float padding;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : UV0;
};

float4 Main(PSInput input) : SV_TARGET
{
    float4 out_color = float4(base_color, 1.0f);
    return out_color;
}
