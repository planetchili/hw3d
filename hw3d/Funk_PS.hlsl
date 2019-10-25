Texture2D tex;
SamplerState splr;

float4 main(float2 uv : Texcoord) : SV_Target
{
    return 1.0f - tex.Sample(splr, uv).rgba;
}