cbuffer ShadowTransformCBuf : register(b1)
{
    matrix shadowPos;
};

float4 ToShadowHomoSpace(const in float3 pos, uniform matrix modelTransform)
{    
    const float4 world = mul(float4(pos, 1.0f), modelTransform);
    return mul(world, shadowPos);
}