Texture2D smap : register(t3);
SamplerState ssam : register(s1);

float Shadow(const in float4 shadowHomoPos)
{    
    float shadowLevel = 0.0f;
    const float3 spos = shadowHomoPos.xyz / shadowHomoPos.w;
    
    if( spos.z > 1.0f )
    {
        shadowLevel = 1.0f;
    }
    else
    {
        // dimensions for pixel sampling with uv
        uint width, height;
        smap.GetDimensions(width, height);
        const float dx = 0.5f / width;
        const float dy = 0.5f / height;
        // sample 4 pixels around actual shadow map position
        const float zBiased = spos.z - 0.00005f; 
        shadowLevel += smap.Sample(ssam, spos.xy + float2(dx, dy)).r >= zBiased ? 0.25f : 0.0f;
        shadowLevel += smap.Sample(ssam, spos.xy + float2(-dx, dy)).r >= zBiased ? 0.25f : 0.0f;
        shadowLevel += smap.Sample(ssam, spos.xy + float2(dx, -dy)).r >= zBiased ? 0.25f : 0.0f;
        shadowLevel += smap.Sample(ssam, spos.xy + float2(-dx, -dy)).r >= zBiased ? 0.25f : 0.0f;
    }
    return shadowLevel;
}