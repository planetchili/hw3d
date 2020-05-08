Texture2D smap : register(t3);
SamplerComparisonState ssam : register(s1);

#define PCF_RANGE 2

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
        for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
        {
            [unroll]
            for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
            {
                shadowLevel += smap.SampleCmpLevelZero(ssam, spos.xy, spos.b - 0.0005f, int2(x, y));
            }
        }       
        shadowLevel /= ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
    }
    return shadowLevel;
}