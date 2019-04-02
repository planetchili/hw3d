cbuffer CBuf
{
	float4 color;
};

float4 main() : SV_Target
{
	return color;
}