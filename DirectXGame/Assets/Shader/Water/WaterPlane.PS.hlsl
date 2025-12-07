
struct VSOutput
{
    float4 posH : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD1;
    float3 viewDir : TEXCOORD2;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

struct DirectionalLight
{
    float4 color;
    float3 direction;
    float intensity;
};

cbuffer Data : register(b0)
{
    DirectionalLight dirLight;
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    // Schlick approximation
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float4 HalfLambert(float3 normal, float4 color, DirectionalLight light)
{
    float NdotL = dot(normalize(normal), -light.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * light.color * cos * light.intensity;
    outputColor.w = color.w;
    return outputColor;
}

PSOutput main(VSOutput input)
{
    //いったん
    PSOutput output;
    output.color = float4(0.5f, 0.7f, 0.9f, 1.0f);
    output.color += HalfLambert(input.normal, output.color, dirLight);
    return output;
}
