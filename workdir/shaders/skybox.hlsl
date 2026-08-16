static const float SUN_FLARE_START_ANGLE = 1.0f;
static const float SUN_FLARE_END_ANGLE = SUN_FLARE_START_ANGLE + 2.5f;
static const float4 SUN_FLARE_COLOR = int4(255, 246, 176, 255) / 255.0f;

float4x4 VP;
float3 SUN_DIRECTION;

TextureCube SKYBOX_TEXTURE : register(t0);

SamplerState TEXTURE_SAMPLER : register(s0);

struct VData
{
    float4 position : SV_Position;
    float3 uvw : VS_UVW;
};

VData v_shader(float3 position : KL_Position)
{
    VData data;
    data.position = mul(float4(position, 0.0f), VP).xyww;
    data.uvw = position;
    return data;
}

float4 p_shader(VData data) : SV_Target
{
    const float4 skybox_color = SKYBOX_TEXTURE.Sample(TEXTURE_SAMPLER, data.uvw);
    const float angle_from_sun = degrees(acos(dot(normalize(data.uvw), -SUN_DIRECTION)));
    if (angle_from_sun < SUN_FLARE_START_ANGLE)
        return 1.0f;
    if (angle_from_sun < SUN_FLARE_END_ANGLE)
        return lerp(SUN_FLARE_COLOR, skybox_color, smoothstep(SUN_FLARE_START_ANGLE, SUN_FLARE_END_ANGLE, angle_from_sun));
    return skybox_color;
}
