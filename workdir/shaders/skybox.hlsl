// Vertex shader
struct v_out
{
    float4 world_position : SV_Position;
    float3 texture_coords : Transfer_Texture;
};

cbuffer SB_CB : register(b0)
{
    matrix vp_matrix;
}

v_out v_shader(const float3 position : KL_Position)
{
    v_out data;
    data.world_position = mul(float4(position, 0.0f), vp_matrix).xyww;
    data.texture_coords = position;
    return data;
}

// Pixel shader
SamplerState box_sampler : register(s0);
TextureCube box_texture : register(t0);

float4 p_shader(const v_out data) : SV_Target
{
    return box_texture.Sample(box_sampler, data.texture_coords);
}
