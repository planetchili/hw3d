Texture2D smap : register(t3);
SamplerState ssam : register(s1);

bool ShadowUnoccluded(const in float3 shadowScreenPos)
{
    return smap.Sample(ssam, shadowScreenPos.xy).r > shadowScreenPos.z - 0.005f;
}