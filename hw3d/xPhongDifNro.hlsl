#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding[1];
};

#include "Transform.hlsl"

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main( float3 viewFragPos : Position,float3 viewNormal : Normal,float2 tc : Texcoord ) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        // sample and unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        const float3 objectNormal = normalSample * 2.0f - 1.0f;
        // bring normal from object space into view space
        viewNormal = normalize(mul(objectNormal, (float3x3) modelView));
    }
	// fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	// specular
    const float3 specular = Speculate(
        specularIntensity.rrr, 1.0f, viewNormal, lv.vToL,
        viewFragPos, att, specularPower
    );
	// final color
	return float4( saturate( (diffuse + ambient) * tex.Sample( splr, tc ).rgb + specular ), 1.0f );
}