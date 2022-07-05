// Vertex shader
float4 vShader(float3 pos : POS_IN) : SV_POSITION
{
    return float4(pos, 1.0f);
}

// Pixel shader
Texture2D colorMap : register(t0);
Texture2D indexMap : register(t1);

float4 pShader(float4 screen : SV_POSITION) : SV_TARGET
{
    int foundCount = 0;
    const int searchRadius = 4;
    const int maxSearchCount = (searchRadius * 2 + 1) * (searchRadius * 2 + 1);
    for (int y = -searchRadius; y <= searchRadius; y++)
    {
        for (int x = -searchRadius; x <= searchRadius; x++)
        {
            if (indexMap[screen.xy + float2(x, y)].r > 0.0f)
            {
                foundCount += 1;
            }
        }
    }
    
    const float4 atmosphereColor = { 0.53f, 0.8f, 0.92f, 1.0f };
    if (foundCount == maxSearchCount)
    {
        return lerp(colorMap[screen.xy], atmosphereColor, 0.075f);
    }
    if (foundCount > 0 && foundCount < maxSearchCount)
    {
        return lerp(colorMap[screen.xy], atmosphereColor, ((float) foundCount) / maxSearchCount * 0.4f);
    }
    return colorMap[screen.xy];
}
