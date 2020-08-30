TextureCube smap : register(t3);
SamplerComparisonState ssam : register(s1);
SamplerState dgsam : register(s2);

float ShadowLoop_(const in float4 spos)
{
    float len = length(spos.xyz);
    float4 checkme = smap.Sample(dgsam, normalize(spos.xyz));
    if( checkme.r < 1000.0f )
    {
        return smap.SampleCmpLevelZero(ssam, normalize(spos.xyz), len / 100.0f);
    }
    else
    {
        return 4.0f;
    }
}

float Shadow(const in float4 shadowPos)
{
    return ShadowLoop_(shadowPos);
}