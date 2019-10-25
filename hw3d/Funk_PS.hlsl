Texture2D tex;
SamplerState splr;

static const int r = 3;
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 uv : Texcoord) : SV_Target
{
    uint width, height;
    tex.GetDimensions(width, height);
    const float dx = 1.0f / width;
    const float dy = 1.0f / height;
    float4 acc = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int y = -r; y <= r; y++)
    {
        for (int x = -r; x <= r; x++)
        {
            const float2 tc = uv + float2(dx * x, dy * y);
            acc += tex.Sample(splr, tc).rgba;
        }
    }
    return acc / divisor;
}