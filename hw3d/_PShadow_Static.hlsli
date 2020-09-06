TextureCube smap : register(t3);
SamplerComparisonState ssam : register(s1);

float Shadow(const in float4 shadowPos)
{
    return 1.0f;
    //smap.SampleCmpLevelZero(ssam, normalize(shadowPos.xyz), length(shadowPos.xyz) / 100.0f);
}