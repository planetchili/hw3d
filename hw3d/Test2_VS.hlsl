struct Out
{
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

Out main(float3 pos : POSITION)
{
    Out vsout;
    vsout.pos = float4(pos, 1.0f);
    vsout.color = float4(0.0f,1.0f,0.0f,1.0f);
    return vsout;
}