#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"
#include "PShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);

SamplerState splr : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    // shadow map test
    if (ShadowUnoccluded(spos))
    {
        // renormalize interpolated normal
        viewNormal = normalize(viewNormal);
	    // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	    // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	    // diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	    // specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lv.vToL, viewFragPos, att, specularGloss);
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}