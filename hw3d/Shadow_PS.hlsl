float2 main(float4 pos : SV_Position, float4 viewPos : Position) : SV_TARGET
{	
    return float2(pos.z, viewPos.w / pos.w);
}