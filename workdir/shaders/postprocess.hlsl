// Vertex shader
float4 v_shader(const float3 position : KL_Position) : SV_Position
{
    return float4(position, 1.0f);
}

// Pixel shader
cbuffer PS_CB
{
    float4 misc_data;
};

Texture2D color_map : register(t0);
Texture2D index_map : register(t1);

float4 p_shader(const float4 screen : SV_Position) : SV_Target
{
    // Atmosphere
    if (!index_map[screen.xy].r) {
        int found_count = 0;
        const int search_radius = 6;
        for (int y = -search_radius; y <= search_radius; y++) {
            for (int x = -search_radius; x <= search_radius; x++) {
                if (index_map[screen.xy + float2(x, y)].r) {
                    found_count += 1;
                }
            }
        }
        const int max_search_count = (search_radius * 2 + 1) * (search_radius * 2 + 1);
        return lerp(color_map[screen.xy], float4(0.5f, 0.8f, 0.9f, 1.0f), found_count / (max_search_count * 1.75f));
    }
    
    // Boundaries
    if (misc_data.x && index_map[screen.xy].g) {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    // Index
    if (index_map[screen.xy].b) {
        return color_map[screen.xy] * (misc_data.y ? 2.0f : 1.5f);
    }
    
    // Default
    return color_map[screen.xy];
}
