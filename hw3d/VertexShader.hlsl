struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer CBuf
{
	matrix transform;
};

VSOut main( float2 pos : Position,float3 color : Color )
{
	VSOut vso;
	vso.pos = mul( float4(pos.x,pos.y,0.0f,1.0f),transform );
	vso.color = color;
	return vso;
}