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

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};
StructuredBuffer<DirectionalLight> gDirectionalLights : register(t1);

float4 HalfLambertPoint(float3 worldPosition, float3 normal, float4 color, PointLight light)
{
    float3 direction = normalize(worldPosition - light.position);
    float NdotL = dot(normalize(normal), -direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * light.color * cos * light.intensity;
    
    float distance = length(worldPosition - light.position);
    
    float power = 1.0f - (distance / 10.0f - light.intensity);
    power = clamp(power, 0.0f, 1.0f);
    
    outputColor.rgb *= power;
    
    outputColor.a = color.a;
    return outputColor;
}

float4 LambertReflectanceDirectional(float3 normal, float4 color, DirectionalLight directionalLight)
{
    float NdotL = dot(normalize(normal), -directionalLight.direction);
    float cos = saturate(dot(normalize(normal), -directionalLight.direction));
    float4 outputColor = color * directionalLight.color * cos * directionalLight.intensity;
    outputColor.w = color.w;
    return outputColor;
}


PSOutput main(PSInput input)
{
    const int kPointLightNum = 64;
    const int kDirectionalLightNum = 3;

    PSOutput output;
    
    float outlineWidth = 0.05f;
    
    float4 origin = input.color;
    
    //Outline
    if (input.texCoord.x < outlineWidth || input.texCoord.x > 1.0f - outlineWidth ||
       input.texCoord.y < outlineWidth || input.texCoord.y > 1.0f - outlineWidth)
    {
        origin = input.outlineColor;
    }
    
    //Easy Alpha Test
    if (origin.a < 0.01f)
    {
        discard;
    }
    
    //PointLight
    float4 sumColor = 0;
    for (int i = 0; i < kPointLightNum; i++)
    {
        sumColor += HalfLambertPoint(input.worldPosition, input.normal, origin, gPointLights[i]);
    }
    
    for (int j = 0; j < kDirectionalLightNum; j++)
    {
        sumColor += LambertReflectanceDirectional(input.normal, origin, gDirectionalLights[j]);
    }
    
    sumColor = clamp(sumColor, 0.0f, 1.0f);
    
    output.color = sumColor;
    
    return output;
}
