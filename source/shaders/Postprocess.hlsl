// Vertex shader
float4 vShader(float3 pos : POS_IN) : SV_POSITION
{
    return float4(pos, 1.0f);
}

// Pixel shader
cbuffer PS_CB
{
    float4 miscData;
};

Texture2D colorMap : register(t0);
Texture2D indexMap : register(t1);

float4 pShader(float4 screen : SV_POSITION) : SV_TARGET
{
    // Amtosphere
    if (!indexMap[screen.xy].r)
    {
        int foundCount = 0;
        const int searchRadius = 6;
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
        const int maxSearchCount = (searchRadius * 2 + 1) * (searchRadius * 2 + 1);
        return lerp(colorMap[screen.xy], float4(0.5f, 0.8f, 0.9f, 1.0f), foundCount / (maxSearchCount * 1.75f));
    }
    
    // Boundaries
    if (miscData.x && indexMap[screen.xy].g)
    {
        return float4(1.0f, 0.55f, 0.0f, 1.0f);
    }
    
    // Index
    if (indexMap[screen.xy].b)
    {
        return colorMap[screen.xy] * (miscData.y ? 1.7f : 1.3f);
    }
    
    // Default
    return colorMap[screen.xy];
}
