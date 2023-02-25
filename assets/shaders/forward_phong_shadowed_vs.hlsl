
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

struct VSInput
{
    float3 pos : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : UV0;
    float3 tangents : TANGENTS0;
};

struct VSOutput
{
    float4 pos_cs : SV_POSITION;
    float4 pos_ws : POSITION0;
    float4 normal_ws : NORMAL0;
    float4 tangents_ws : TANGENT0;
    float2 uv : UV0;
    float depth_cs : DEPTHCS;
};

VSOutput Main(VSInput input)
{
    VSOutput output;
    float4x4 mat_world_view = mul(mat_world, mat_view);
    float4x4 mat_world_view_projection = mul(mat_world, mat_view_projection);

    output.pos_ws = mul(float4(input.pos, 1.0f), mat_world);

    output.pos_cs = mul(float4(output.pos_ws.xyz, 1.0f), mat_view_projection);

    output.depth_cs = output.pos_cs.w;

    output.normal_ws = mul(float4(input.normal, 0.0f), mat_world);  // If we want to support non-uniform scalings we'll need a normal matrix.
                                                                    // Or: https://lxjk.github.io/2017/10/01/Stop-Using-Normal-Matrix.html
    output.tangents_ws = mul(float4(input.tangents, 0.0f), mat_world);
    output.uv = input.uv;
    return output;
}