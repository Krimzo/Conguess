static const int ATMOSHPERE_SEARCH_RADIUS = 6;
static const float4 ATMOSHPERE_COLOR = { 0.5f, 0.8f, 0.9f, 1.0f };
static const float ATMOSHPERE_LERP_MULTI = 1.75f;

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
    int found_count = 0;
    for (int y = -ATMOSHPERE_SEARCH_RADIUS; y <= ATMOSHPERE_SEARCH_RADIUS; y++)
    {
        for (int x = -ATMOSHPERE_SEARCH_RADIUS; x <= ATMOSHPERE_SEARCH_RADIUS; x++)
        {
            if (INFO_TEXTURE[screen + float2(x, y)].r)
                found_count += 1;
        }
    }
    static const int MAX_SEARCH_COUNT = (ATMOSHPERE_SEARCH_RADIUS * 2 + 1) * (ATMOSHPERE_SEARCH_RADIUS * 2 + 1);
    const float lerp_val = found_count / (MAX_SEARCH_COUNT * ATMOSHPERE_LERP_MULTI);
    return lerp(RENDER_TEXTURE[screen], ATMOSHPERE_COLOR, lerp_val);
}

float4 render_borders(float2 screen)
{
    return BORDER_COLOR;
}

float4 render_mouse_country(float2 screen)
{
    return RENDER_TEXTURE[screen] * (MOUSE_LMB ? HOLD_COUNTRY_COLOR_MULTI : HOVER_COUNTRY_COLOR_MULTI);
}

float4 p_shader(float4 screen : SV_Position) : SV_Target
{
    if (RENDER_ATMOSPHERE && !INFO_TEXTURE[screen.xy].r)
        return render_atmosphere(screen.xy);
    
    if (RENDER_BORDERS && INFO_TEXTURE[screen.xy].g)
        return render_borders(screen.xy);
    
    if (INFO_TEXTURE[screen.xy].b)
        return render_mouse_country(screen.xy);
    
    return RENDER_TEXTURE[screen.xy];
}
