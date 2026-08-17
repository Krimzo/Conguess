static const int ATMOSHPERE_SEARCH_RADIUS = 6;
static const float4 ATMOSHPERE_COLOR = { 0.5f, 0.8f, 0.9f, 1.0f };

float4 HIGHLIGHT_COUNTRY_COLOR_MULTI;
float4 HOVER_COUNTRY_COLOR_MULTI;
float4 HOLD_COUNTRY_COLOR_MULTI;
float4 BORDER_COLOR;
float RENDER_BORDERS;
float RENDER_ATMOSPHERE;
float MOUSE_LMB;

Texture2D RENDER_TEXTURE : register(t0);
Texture2D INFO_TEXTURE : register(t1);

float4 v_shader(float3 position : KL_Position) : SV_Position
{
    return float4(position, 1.0f);
}

float4 render_atmosphere(float2 screen)
{
    static const int MAX_SEARCH_COUNT = (ATMOSHPERE_SEARCH_RADIUS * 2 + 1) * (ATMOSHPERE_SEARCH_RADIUS * 2 + 1);
    int found_count = 0;
    float diffuse_sum = 0.0f;
    for (int y = -ATMOSHPERE_SEARCH_RADIUS; y <= ATMOSHPERE_SEARCH_RADIUS; y++)
    {
        for (int x = -ATMOSHPERE_SEARCH_RADIUS; x <= ATMOSHPERE_SEARCH_RADIUS; x++)
        {
            const float4 info = INFO_TEXTURE[screen + float2(x, y)];
            if (!info.r)
                continue;
            found_count += 1;
            diffuse_sum += info.a;
        }
    }
    if (diffuse_sum > 0.0f)
    {
        const float lerp_val = found_count / float(MAX_SEARCH_COUNT);
        float4 result = lerp(RENDER_TEXTURE[screen], ATMOSHPERE_COLOR, lerp_val);
        result.xyz *= diffuse_sum / found_count;
        return result;
    }
    else
        return RENDER_TEXTURE[screen];
}

float4 render_borders(float2 screen)
{
    return BORDER_COLOR;
}

float4 render_highlight_country(float2 screen)
{
    return RENDER_TEXTURE[screen] * HIGHLIGHT_COUNTRY_COLOR_MULTI;
}

float4 render_mouse_country(float2 screen)
{
    return RENDER_TEXTURE[screen] * (MOUSE_LMB ? HOLD_COUNTRY_COLOR_MULTI : HOVER_COUNTRY_COLOR_MULTI);
}

float4 p_shader(float4 screen : SV_Position) : SV_Target
{
    const float4 info_data = INFO_TEXTURE[screen.xy];
    
    if (RENDER_ATMOSPHERE && !info_data.r)
        return render_atmosphere(screen.xy);
    
    if (RENDER_BORDERS && info_data.g)
        return render_borders(screen.xy);
    
    if (info_data.b == 1.0f)
        return render_highlight_country(screen.xy);
    
    if (info_data.b == 2.0f)
        return render_mouse_country(screen.xy);
    
    return RENDER_TEXTURE[screen.xy];
}
