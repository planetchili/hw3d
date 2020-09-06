#include "Transform.hlsli"

struct Output
{
    float3 viewPos : Position;
    float4 pos : SV_Position;
};

Output main(float3 pos : Position)
{
    Output output;
	output.pos = mul(float4(pos, 1.0f), modelViewProj);
	output.viewPos = mul(float4(pos, 1.0f), modelView).xyz;
	return output;
}