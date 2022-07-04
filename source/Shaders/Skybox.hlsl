// Vertex shader
struct vOut
{
    float4 world : SV_POSITION;
    float3 textur : TEX;
};

cbuffer SB_CB : register(b0)
{
    matrix vp;
}

vOut vShader(float3 pos : POS_IN, float2 tex : TEX_IN)
{
    vOut data;
    data.world = mul(float4(pos, 0.0f), vp).xyww;
    data.textur = pos;
    return data;
}

// Pixel shader
SamplerState boxSampler : register(s0);
TextureCube boxTexture : register(t0);

float4 pShader(vOut data) : SV_TARGET
{
    return boxTexture.Sample(boxSampler, data.textur);
}
