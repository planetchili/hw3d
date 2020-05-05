struct LightVectorData
{
    float3 vToL;
    float3 dirToL;
    float distToL;
};

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 fragPos)
{
    LightVectorData lv;
    lv.vToL = lightPos - fragPos;
    lv.distToL = length(lv.vToL);
    lv.dirToL = lv.vToL / lv.distToL;
    return lv;
}