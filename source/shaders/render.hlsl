// Vertex shader
cbuffer vs_cb : register(b0)
{
    matrix w_matrix;
    matrix vp_matrix;
}

struct vs_out
{
    float4 screen_position : SV_Position;
    float3 world_position : Transfer_World;
    float2 texture_coords : Transfer_Texture;
    float3 normal_vector : Transfer_Normal;
};

vs_out v_shader(const float3 position : KL_Position, const float2 texture_coords : KL_Texture, const float3 normal : KL_Normal)
{
    vs_out data;
    data.world_position = mul(float4(position, 1.0f), w_matrix).xyz;
    data.screen_position = mul(float4(data.world_position, 1.0f), vp_matrix);
    data.texture_coords = texture_coords;
    data.normal_vector = mul(float4(normal, 0.0f), w_matrix).xyz;
    return data;
}

// Pixel shader
cbuffer ps_cb : register(b0)
{
    float4 sun_direction;
    float4 camera_position;
    float4 misc_data;
}

SamplerState default_sampler : register(s0);
Texture2D earth_day_map : register(t0);
Texture2D earth_night_map : register(t1);
Texture2D earth_clouds_map : register(t2);
Texture2D earth_normal_map : register(t3);
Texture2D earth_roughness_map : register(t4);
Texture2D earth_boundaries_map : register(t5);
Texture2D earth_indices_map : register(t6);

struct ps_out
{
    float4 color : SV_Target0;
    float4 index : SV_Target1;
};

float3 get_frag_normal(const float3 frag_position, float3 frag_normal, const float2 frag_texture)
{
    const float3 q1 = ddx(frag_position);
    const float3 q2 = ddy(frag_position);
    const float2 st1 = ddx(frag_texture);
    const float2 st2 = ddy(frag_texture);
    const float3 t = normalize(q1 * st2.x - q2 * st1.x);
    const float3 b = normalize(-q1 * st2.y + q2 * st1.y);
    const float3x3 tbn = float3x3(t, b, frag_normal);
    const float3 new_normal = normalize(earth_normal_map.Sample(default_sampler, frag_texture).xyz * 2.0f - 1.0f);
    return normalize(mul(new_normal, tbn));
}

float get_frag_roughness(const float2 frag_texture)
{
    return earth_roughness_map.Sample(default_sampler, frag_texture).x;
}

int from4_values(const float value)
{
    if (value < 0.2f) {
        return 0;
    }
    if (value < 0.5f) {
        return 1;
    }
    if (value < 0.8f) {
        return 2;
    }
    return 3;
}

int from4_values(const float4 data)
{
    static const int powers[4] = { 64, 16, 4, 1 };
    int value = 0;
    for (int i = 0; i < 4; i++) {
        value += from4_values(data[i]) * powers[i];
    }
    return value;
}

ps_out p_shader(vs_out data)
{
    data.normal_vector = get_frag_normal(data.world_position, normalize(data.normal_vector), data.texture_coords);
    
    const float ambient_factor = 0.05f;
    const float3 ambient_color = ambient_factor;

    const float diffuse_factor = max(dot(-sun_direction.xyz, data.normal_vector), 0.0f);
    const float3 diffuse_component = { diffuse_factor, diffuse_factor, diffuse_factor };
    
    const float specular_strength = 1.0f - get_frag_roughness(data.texture_coords);
    const float3 view_direction = normalize(camera_position.xyz - data.world_position);
    const float3 reflection_direction = reflect(sun_direction.xyz, data.normal_vector);
    const float specular_factor = pow(max(dot(view_direction, reflection_direction), 0.0f), 16.0f);
    const float3 specular_component = specular_strength * specular_factor;

    const float4 full_light = float4(diffuse_component + specular_component + ambient_color, 1.0f);
    
    const float4 day_color = earth_day_map.Sample(default_sampler, data.texture_coords);
    const float4 night_color = earth_night_map.Sample(default_sampler, data.texture_coords);
    
    const float2 new_cloud_coords = float2(data.texture_coords.x - misc_data.x, data.texture_coords.y);
    const float4 cloud_color = earth_clouds_map.Sample(default_sampler, new_cloud_coords);
    
    const float4 bounds_color = earth_boundaries_map.Sample(default_sampler, data.texture_coords);
    
    const float4 index_color = earth_indices_map.Sample(default_sampler, data.texture_coords);
    const float in_mouse_country = (from4_values(index_color) == int(misc_data.y)) ? 1.0f : 0.0f;
    
    float4 final_color = day_color * full_light;
    final_color = lerp(night_color, final_color, diffuse_factor);
    final_color = lerp(final_color, cloud_color, misc_data.z ? cloud_color.r : 0.0f);
    
    const float4 final_index = { 1.0f, bounds_color.r, in_mouse_country, 0.0f };
    
    ps_out out_data;
    out_data.color = final_color;
    out_data.index = final_index;
    return out_data;
}
