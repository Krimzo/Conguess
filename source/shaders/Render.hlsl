// Vertex shader
cbuffer VS_CB : register(b0)
{
    matrix wMatrix;
    matrix vpMatrix;
}

struct VS_OUT
{
    float4 screen : SV_POSITION;
    float3 world : WRLD;
    float2 textur : TEX;
    float3 normal : NORM;
};

VS_OUT vShader(float3 pos : POS_IN, float2 tex : TEX_IN, float3 norm : NORM_IN)
{
    VS_OUT data;
    
    data.world = mul(float4(pos, 1.0f), wMatrix).xyz;
    data.screen = mul(float4(data.world, 1.0f), vpMatrix);
    data.textur = tex;
    data.normal = mul(float4(norm, 0.0f), wMatrix).xyz;
    
    return data;
}

// Pixel shader
cbuffer PS_CB : register(b0)
{
    float4 sunDirection;
    float4 cameraPosition;
    float4 miscData;
}

SamplerState defaultSampler : register(s0);
Texture2D earthDayMap : register(t0);
Texture2D earthNightMap : register(t1);
Texture2D earthCloudsMap : register(t2);
Texture2D earthNormalMap : register(t3);
Texture2D earthRoughnessMap : register(t4);
Texture2D earthBoundariesMap : register(t5);

float3 GetFragNormal(float3 fragPosition, float3 fragNormal, float2 fragTexture)
{
    const float3 Q1 = ddx(fragPosition);
    const float3 Q2 = ddy(fragPosition);
    const float2 st1 = ddx(fragTexture);
    const float2 st2 = ddy(fragTexture);
    const float3 T = normalize(Q1 * st2.x - Q2 * st1.x);
    const float3 B = normalize(-Q1 * st2.y + Q2 * st1.y);
    const float3x3 TBN = float3x3(T, B, fragNormal);
    float3 newNormal = normalize(earthNormalMap.Sample(defaultSampler, fragTexture).xyz * 2.0f - 1.0f);
    return normalize(mul(newNormal, TBN));
}
float GetFragRoughnes(float2 fragTexture)
{
    return earthRoughnessMap.Sample(defaultSampler, fragTexture).x;
}

struct PS_OUT
{
    float4 color : SV_TARGET0;
    float4 index : SV_TARGET1;
};

PS_OUT pShader(VS_OUT data)
{
    data.normal = GetFragNormal(data.world, normalize(data.normal), data.textur);
    
    const float ambientFactor = 0.05f;
    const float3 ambientColor = { ambientFactor, ambientFactor, ambientFactor };

    const float diffuseFactor = max(dot(-sunDirection.xyz, data.normal), 0.0f);
    const float3 diffuseComponent = { diffuseFactor, diffuseFactor, diffuseFactor };
    
    const float specularStrength = 1.0f - GetFragRoughnes(data.textur);
    const float3 viewDirection = normalize(cameraPosition.xyz - data.world);
    const float3 reflectionDirection = reflect(sunDirection.xyz, data.normal);
    const float specularFactor = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16.0f);
    const float3 specularComponent = specularStrength * specularFactor;

    const float4 fullLight = float4(diffuseComponent + specularComponent + ambientColor, 1.0f);
    
    const float4 dayColor = earthDayMap.Sample(defaultSampler, data.textur);
    const float4 nightColor = earthNightMap.Sample(defaultSampler, data.textur);
    
    const float2 newCloudCoords = float2(data.textur.x - miscData.x, data.textur.y);
    const float4 cloudColor = earthCloudsMap.Sample(defaultSampler, newCloudCoords);
    
    const float4 boundsColor = earthBoundariesMap.Sample(defaultSampler, data.textur);
    
    float4 finalColor = dayColor * fullLight;
    finalColor = lerp(nightColor, finalColor, diffuseFactor);
    finalColor = lerp(finalColor, cloudColor, cloudColor.r);
    finalColor = lerp(finalColor, boundsColor, boundsColor.r);
    
    const float4 index = { 1.0f, boundsColor.r, 0.0f, 0.0f };
    
    PS_OUT outData;
    outData.color = finalColor;
    outData.index = index;
    return outData;
}
