TextureCube smap : register(t3);
SamplerComparisonState ssam : register(s1);

static const float zf = 100.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float CalculateShadowDepth(const in float4 shadowPos)
{
    const float3 d = abs(shadowPos).xyz;
    return (c1 * max(d.x, max(d.y, d.z)) + c0) / d.z;
}

float Shadow(const in float4 shadowPos)
{
    return smap.SampleCmpLevelZero(ssam, shadowPos.xyz, CalculateShadowDepth(shadowPos));
}