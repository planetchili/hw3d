Texture2D tex;
SamplerState splr;

cbuffer Kernel
{
    uint nTaps;
    float coefficients[15];
}

cbuffer Control
{
    bool horizontal;
}

float4 main(float2 uv : Texcoord) : SV_Target
{
    float width, height;
    tex.GetDimensions(width, height);
    float dx, dy;
    if (horizontal)
    {
        dx = 1.0f / width;
        dy = 0.0f;
    }
    else
    {
        dx = 0.0f;
        dy = 1.0f / height;
    }
    const int r = nTaps / 2;
    
    float accAlpha = 0.0f;
    float3 maxColor = float3(0.0f, 0.0f, 0.0f);
    for (int i = -r; i <= r; i++)
    {
        const float2 tc = uv + float2(dx * i, dy * i);
        const float4 s = tex.Sample(splr, tc).rgba;
        const float coef = coefficients[i + r];
        accAlpha += s.a * coef;
        maxColor = max(s.rgb, maxColor);
    }
    return float4(maxColor, accAlpha);
}