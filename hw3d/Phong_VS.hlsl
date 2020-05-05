#include "Transform.hlsli"
#include "VShadow.hlsli"

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float4 shadowHomoPos : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.shadowHomoPos = ToShadowHomoSpace(pos, model);
    return vso;
}