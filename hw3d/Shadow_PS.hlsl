float main(float4 pos : SV_Position, float4 viewPos : Position) : SV_TARGET
{	
    float bias = 0.0005 + (pos.z - viewPos.w / pos.w);
    return saturate(length(viewPos.xyz) / 100.0f + bias);
}