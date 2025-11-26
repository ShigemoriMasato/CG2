#include "PointLightBlock.hlsli"

struct PSOutput
{
    float4 color : SV_Target0;
};

struct PointLight
{
    float4 color;
    float3 position;
    float intensity;
};
StructuredBuffer<PointLight> gPointLights : register(t0);

const int kPointLightNum = 8;

float4 HalfLambert(float3 worldPosition, float3 normal, float4 color, PointLight light)
{
    float3 direction = normalize(worldPosition - light.position);
    float NdotL = dot(normal, -direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * light.color * cos * light.intensity;
    outputColor.w = color.w;
    return outputColor;
}


PSOutput main(PSInput input)
{
    PSOutput output;
    
    float outlineWidth = 0.05f;
    
    output.color = input.color;
    
    //Outline
    if (input.texCoord.x < outlineWidth || input.texCoord.x > 1.0f - outlineWidth ||
       input.texCoord.y < outlineWidth || input.texCoord.y > 1.0f - outlineWidth)
    {
        output.color = input.outlineColor;
    }
    
    //Easy Alpha Test
    if (output.color.a < 0.01f)
    {
        discard;
    }
    
    //PointLight
    for (int i = 0; i < kPointLightNum; i++)
    {
        output.color += HalfLambert(input.worldPosition, input.normal, input.color, gPointLights[i]);
    }
    
    return output;
}
