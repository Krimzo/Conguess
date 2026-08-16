float4x4 W;
float4x4 VP;
float3 SUN_DIRECTION;
float ELAPSED_TIME;
float3 CAMERA_POSITION;
float RENDER_CLOUDS;
int MOUSE_COUNTRY;

Texture2D EARTH_DAY_TEXTURE : register(t0);
Texture2D EARTH_NIGHT_TEXTURE : register(t1);
Texture2D EARTH_CLOUDS_TEXTURE : register(t2);
Texture2D EARTH_NORMAL_TEXTURE : register(t3);
Texture2D EARTH_ROUGHNESS_TEXTURE : register(t4);
Texture2D EARTH_BORDERS_TEXTURE : register(t5);
Texture2D EARTH_INDICES_TEXTURE : register(t6);

SamplerState LINEAR_SAMPLER : register(s0);
SamplerState DIRECT_SAMPLER : register(s1);

struct VData
{
    float4 position : SV_Position;
    float3 world_position : VS_World;
    float3 normal : VS_Normal;
    float2 uv : VS_UV;
};

struct PData
{
    float4 color : SV_Target0;
    float4 info : SV_Target1;
};

VData v_shader(float3 position : KL_Position, float3 normal : KL_Normal, float2 uv : KL_UV)
{
    VData data;
    data.world_position = mul(float4(position, 1.0f), W).xyz;
    data.position = mul(float4(data.world_position, 1.0f), VP);
    data.normal = mul(float4(normal, 0.0f), W).xyz;
    data.uv = uv;
    return data;
}

float3 get_frag_normal(float3 frag_position, float3 frag_normal, float2 frag_texture)
{
    const float3 q1 = ddx(frag_position);
    const float3 q2 = ddy(frag_position);
    const float2 st1 = ddx(frag_texture);
    const float2 st2 = ddy(frag_texture);
    const float3 t = normalize(q1 * st2.x - q2 * st1.x);
    const float3 b = normalize(-q1 * st2.y + q2 * st1.y);
    const float3x3 tbn = float3x3(t, b, frag_normal);
    const float3 new_normal = normalize(EARTH_NORMAL_TEXTURE.Sample(LINEAR_SAMPLER, frag_texture).xyz * 2.0f - 1.0f);
    return normalize(mul(new_normal, tbn));
}

float get_frag_roughness(float2 frag_texture)
{
    return EARTH_ROUGHNESS_TEXTURE.Sample(LINEAR_SAMPLER, frag_texture).x;
}

PData p_shader(VData data)
{
    data.normal = get_frag_normal(data.world_position, normalize(data.normal), data.uv);
    
    const float3 day_color = EARTH_DAY_TEXTURE.Sample(LINEAR_SAMPLER, data.uv).rgb;
    const float3 night_color = EARTH_NIGHT_TEXTURE.Sample(LINEAR_SAMPLER, data.uv).rgb;
    const float cloud_value = EARTH_CLOUDS_TEXTURE.Sample(LINEAR_SAMPLER, float2(data.uv.x - ELAPSED_TIME, data.uv.y)).r;
    const float border_value = EARTH_BORDERS_TEXTURE.Sample(LINEAR_SAMPLER, data.uv).r;
    
    const int country_index = round(EARTH_INDICES_TEXTURE.Sample(DIRECT_SAMPLER, data.uv).r * 255);
    const float in_mouse_country = (country_index == MOUSE_COUNTRY);
    
    const float ndotl = dot(-SUN_DIRECTION, data.normal);
    const float terminator = smoothstep(-0.25, 0.25, ndotl);
    const float diffuse_factor = saturate(ndotl);
    
    const float specular_strength = 1.0f - get_frag_roughness(data.uv);
    const float3 view_direction = normalize(CAMERA_POSITION - data.world_position);
    const float3 reflection_direction = reflect(SUN_DIRECTION, data.normal);
    const float specular_factor = pow(saturate(dot(view_direction, reflection_direction)), 16.0f) * specular_strength;
    
    const float3 daily_color = lerp(day_color * (diffuse_factor + specular_factor), cloud_value, RENDER_CLOUDS ? cloud_value : 0.0f);
    const float3 nightly_color = night_color * (RENDER_CLOUDS ? (1.0f - cloud_value) : 1.0f);
    const float3 color = lerp(nightly_color, daily_color, terminator);
        
    PData out_data;
    out_data.color = float4(color, 1.0f);
    out_data.info = float4(1.0f, border_value, in_mouse_country, diffuse_factor);
    return out_data;
}
