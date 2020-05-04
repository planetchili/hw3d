#include "Transform.hlsl"
cbuffer ShadowTransform
{
    matrix shadowView;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float2 tc : Texcoord;
    float4 shadowCamPos : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
    const float4 shadowCamera = mul(float4(pos, 1.0f), model);
    const float4 shadowHomo = mul(shadowCamera, shadowView);
    vso.shadowCamPos = shadowHomo * float4(0.5f, -0.5f, 1.0f, 1.0f) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowHomo.w);
    return vso;
}