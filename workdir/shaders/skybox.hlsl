float4x4 VP;

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
    return SKYBOX_TEXTURE.Sample(TEXTURE_SAMPLER, data.uvw);
}
