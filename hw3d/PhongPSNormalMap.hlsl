cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if( normalMapEnabled )
    {
        // build the tranform (rotation) into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(viewNormal)
        );
        // unpack the normal from map into tangent space        
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        float3 tanNormal;
        tanNormal = normalSample * 2.0f - 1.0f;
        tanNormal.y = -tanNormal.y;
        // bring normal from tanspace into view space
        viewNormal = mul(tanNormal, tanToView);
    }
	// fragment to light vector data
    const float3 vToL = lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	// attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, viewNormal));
	// reflected light vector
    const float3 w = viewNormal * dot(vToL, viewNormal);
    const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);
	// final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}