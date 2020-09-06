float4 main(float3 viewPos : Position) : SV_TARGET
{	
    return length(viewPos) / 100.0f + 0.005;
}