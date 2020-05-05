#include "ShaderOps.hlsli"
#include "LightVectorData.hlsli"

#include "PointLight.hlsli"
#include "PShadow.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);

SamplerState splr : register(s0);


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float3 spos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specularReflected;
    
    // shadow map test
    if (ShadowUnoccluded(spos))
    {
        // normalize the mesh normal
        viewNormal = normalize(viewNormal);
	    // fragment to light vector data
        const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
        // specular parameters
        float specularPowerLoaded = specularGloss;
        const float4 specularSample = spec.Sample(splr, tc);
        float3 specularReflectionColor;
        if (useSpecularMap)
        {
            specularReflectionColor = specularSample.rgb;
        }
        else
        {
            specularReflectionColor = specularColor;
        }
        if (useGlossAlpha)
        {
            specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
        }
	    // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	    // diffuse light
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
        // specular reflected
        specularReflected = Speculate(
            diffuseColor * specularReflectionColor, specularWeight, viewNormal,
            lv.vToL, viewFragPos, att, specularPowerLoaded
        );
    }
    else
    {
        diffuse = specularReflected = 0.0f;
    }
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}