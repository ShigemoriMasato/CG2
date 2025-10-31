#include "BackGround.hlsli"

struct PSOutput
{
    float4 color : SV_Target0;
};

cbuffer DirectionalLight : register(b0)
{
    float4 lightColor;
    float3 direction;
    float intensity;
    int enableLighting;
    float3 padding;
};

float4 HalfLambert(float3 normal, float4 color)
{
    float NdotL = dot(normalize(normal), -direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * lightColor * cos * intensity;
    outputColor.w = color.w;
    return outputColor;
}

float4 LambertReflectance(float3 normal, float4 color)
{
    float NdotL = dot(normalize(normal), -direction);
    float cos = saturate(dot(normalize(normal), -direction));
    float4 outputColor = color * lightColor * cos * intensity;
    outputColor.w = color.w;
    return outputColor;
}

//使わんけどRootSignature的に一応
Texture2D tex : register(t0);
SamplerState samp : register(s0);

PSOutput main(PSInput input)
{
    PSOutput output;
    
    if(input.color.a < 0.1f)
    {
        discard;
    }
    
    float outlineWidth = 0.05f;
    
    output.color = input.color;
    
    if (input.texCoord.x < outlineWidth || input.texCoord.x > 1.0f - outlineWidth ||
       input.texCoord.y < outlineWidth || input.texCoord.y > 1.0f - outlineWidth)
    {
        output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    output.color = LambertReflectance(input.normal, output.color);
    
    return output;
}
