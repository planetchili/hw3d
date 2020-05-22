Texture2D smap : register(t3);
SamplerComparisonState ssamHw : register(s1);
SamplerState ssamSw : register(s2);

cbuffer ShadowControl : register(b2)
{
    int pcfLevel;
    float depthBias;
    bool hwPcf;
}

float ShadowLoop_(const in float3 spos, uniform int range)
{    
    float shadowLevel = 0.0f;
    [unroll]
    for (int x = -range; x <= range; x++)
    {
        [unroll]
        for (int y = -range; y <= range; y++)
        {
            if( hwPcf )
            {
                shadowLevel += smap.SampleCmpLevelZero(ssamHw, spos.xy, spos.b - depthBias, int2(x, y));
            }
            else
            {
                shadowLevel += smap.Sample(ssamSw, spos.xy, int2(x, y)).r >= spos.b - depthBias ? 1.0f : 0.0f;
            }
        }
    }
    return shadowLevel / ((range * 2 + 1) * (range * 2 + 1));
}

float Shadow(const in float4 shadowHomoPos)
{    
    float shadowLevel = 0.0f;
    const float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
    
    if( spos.z > 1.0f || spos.z < 0.0f )
    {
        shadowLevel = 1.0f;
    }
    else
    {
        [unroll]
        for (int level = 0; level <= 4; level++)
        {
            if (level == pcfLevel)
            {
                shadowLevel = ShadowLoop_(spos, level);
            }
        }
    }
    return shadowLevel;
}