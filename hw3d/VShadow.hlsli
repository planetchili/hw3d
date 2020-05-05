cbuffer ShadowTransformCBuf : register(b1)
{
    matrix shadowViewProj;
};

float4 ToShadowHomoSpace(const in float3 pos, uniform matrix modelTransform)
{    
    const float4 world = mul(float4(pos, 1.0f), modelTransform);
    const float4 shadowHomo = mul(world, shadowViewProj);
    return shadowHomo * float4(0.5f, -0.5f, 1.0f, 1.0f) + float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowHomo.w;
}