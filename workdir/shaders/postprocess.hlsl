float4 MISC_DATA;

Texture2D RENDER_TEXTURE : register(t0);
Texture2D INDEX_TEXTURE : register(t1);

float4 v_shader(float3 position : KL_Position) : SV_Position
{
    return float4(position, 1.0f);
}

float4 p_shader(float4 screen : SV_Position) : SV_Target
{
    // Atmosphere
    if (!INDEX_TEXTURE[screen.xy].r) {
        int found_count = 0;
        const int search_radius = 6;
        for (int y = -search_radius; y <= search_radius; y++) {
            for (int x = -search_radius; x <= search_radius; x++) {
                if (INDEX_TEXTURE[screen.xy + float2(x, y)].r) {
                    found_count += 1;
                }
            }
        }
        const int max_search_count = (search_radius * 2 + 1) * (search_radius * 2 + 1);
        return lerp(RENDER_TEXTURE[screen.xy], float4(0.5f, 0.8f, 0.9f, 1.0f), found_count / (max_search_count * 1.75f));
    }
    
    // Boundaries
    if (MISC_DATA.x && INDEX_TEXTURE[screen.xy].g) {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    // Index
    if (INDEX_TEXTURE[screen.xy].b) {
        return RENDER_TEXTURE[screen.xy] * (MISC_DATA.y ? 2.0f : 1.5f);
    }
    
    // Default
    return RENDER_TEXTURE[screen.xy];
}
