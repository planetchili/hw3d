#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;

SamplerState splr;


float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample diffuse texture
    float4 dtex = tex.Sample(splr, tc);

    #ifdef MASK_BOI
    // bail if highly translucent
    clip(dtex.a < 0.1f ? -1 : 1);
    // flip normal when backface
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    #endif

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
    // specular parameter determination (mapped or uniform)
    float3 specularReflectionColor;
    float specularPower = specularGloss;
    const float4 specularSample = spec.Sample(splr, tc);
    if( useSpecularMap )
    {
        specularReflectionColor = specularSample.rgb;
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    if( useGlossAlpha )
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	// diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        diffuseColor * diffuseIntensity * specularReflectionColor, specularWeight, viewNormal,
        lv.vToL, viewFragPos, att, specularPower
    );
	// final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * dtex.rgb + specularReflected), 1.0f);
}