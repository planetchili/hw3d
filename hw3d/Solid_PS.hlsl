cbuffer CBuf : register(b1)
{
	float3 materialColor;
};

float4 main() : SV_Target
{
    return float4(materialColor, 1.0f);
}