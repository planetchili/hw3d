#include "Transform.hlsli"

float4 main(float3 pos : Position) : SV_Position
{
	float4 output = mul(float4(pos, 1.0f), modelViewProj);
	// get vtx position relative to light source (oriented relative to current shadow projection direction)
	const float3 viewPos = mul(float4(pos, 1.0f), modelView).xyz;
	// we store z as cartesian distance to make cubemap depth comparison convenient & fast
	// depth stored will be linear actual distance from point light (fp is locked to 100)
	// premultiply by w to counteract the perspective division of z
	
	output.z = length(viewPos) * output.w / 100.0f;
	return output;
}