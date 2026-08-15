float RENDER_BOUNDS;
float MOUSE_LMB;

Texture2D RENDER_TEXTURE : register(t0);
Texture2D INFO_TEXTURE : register(t1);

float4 v_shader(float3 position : KL_Position) : SV_Position
{
    return float4(position, 1.0f);
}

float4 p_shader(float4 screen : SV_Position) : SV_Target
{
    if (!INFO_TEXTURE[screen.xy].r)
    {
        int found_count = 0;
        const int search_radius = 6;
        for (int y = -search_radius; y <= search_radius; y++)
        {
            for (int x = -search_radius; x <= search_radius; x++)
            {
                if (INFO_TEXTURE[screen.xy + float2(x, y)].r)
                    found_count += 1;
            }
        }
        const int max_search_count = (search_radius * 2 + 1) * (search_radius * 2 + 1);
        return lerp(RENDER_TEXTURE[screen.xy], float4(0.5f, 0.8f, 0.9f, 1.0f), found_count / (max_search_count * 1.75f));
    }
    
    if (RENDER_BOUNDS && INFO_TEXTURE[screen.xy].g)
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if (INFO_TEXTURE[screen.xy].b)
        return RENDER_TEXTURE[screen.xy] * (MOUSE_LMB ? 2.0f : 1.5f);
    
    return RENDER_TEXTURE[screen.xy];
}
