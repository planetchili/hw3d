#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"
#include "Pshadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
Texture2D nmap : register(t2);

SamplerState splr : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float3 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    // shadow map test
    if (ShadowUnoccluded(spos))
    {
        // normalize the mesh normal
        viewNormal = normalize(viewNormal);
        // replace normal with mapped if normal mapping enabled
        if (useNormalMap)
        {
            const float3 mappedNormal = MapNormal(normalize(viewTan), normalize(viewBitan), viewNormal, tc, nmap, splr);
            viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
        }
	    // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	    // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	    // diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
        // specular
        specular = Speculate(
            diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal,
            lv.vToL, viewFragPos, att, specularGloss
        );
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}