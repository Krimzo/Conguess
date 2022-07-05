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
    if (!indexMap[screen.xy].r)
    {
        int foundCount = 0;
        const int searchRadius = 6;
        const int maxSearchCount = (searchRadius * 2 + 1) * (searchRadius * 2 + 1);
        for (int y = -searchRadius; y <= searchRadius; y++)
        {
            for (int x = -searchRadius; x <= searchRadius; x++)
            {
                if (indexMap[screen.xy + float2(x, y)].r)
                {
                    foundCount += 1;
                }
            }
        }
        
        const float4 atmosphereColor = { 0.53f, 0.8f, 0.92f, 1.0f };
        if (foundCount > 0)
        {
            return lerp(colorMap[screen.xy], atmosphereColor, foundCount / (maxSearchCount * 1.75f));
        }
    }
    return colorMap[screen.xy];
}
