TextureCube smap : register(t3);
SamplerComparisonState ssam : register(s1);

float ShadowLoop_(const in float4 spos)
{
    return smap.SampleCmpLevelZero(ssam, spos.xyz, length(spos.xyz) / 100.0f);
}

float Shadow(const in float4 shadowPos)
{
    return ShadowLoop_(shadowPos);
}