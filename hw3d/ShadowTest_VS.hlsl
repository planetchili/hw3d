#include "Transform.hlsli"
cbuffer ShadowTransform
{
    matrix shadowView;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float2 tc : Texcoord;
    noperspective float3 shadowCamScreen : ShadowPosition;
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
    vso.shadowCamScreen = (shadowHomo.xyz / shadowHomo.w) * float3(0.5f, -0.5f, 1.0f) + float3(0.5f, 0.5f, 0.0f);
    return vso;
}