#include "Object3d.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    float shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    int enableLight;
    float4 color;
    float3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture[] : register(t0);
SamplerState gSampler : register(s0);


float4 HalfLambert(float3 normal, float4 color, float4 textureColor, DirectionalLight directionalLight)
{
    float NdotL = dot(normalize(normal), -directionalLight.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = color * textureColor * directionalLight.color * cos * directionalLight.intensity;
    outputColor.w = color.w * textureColor.w;
    return outputColor;
}

float4 LambertReflectance(float3 normal, float4 color, float4 textureColor, DirectionalLight directionalLight)
{
    float NdotL = dot(normalize(normal), -directionalLight.direction);
    float cos = saturate(dot(normalize(normal), -directionalLight.direction));
    float4 outputColor = color * textureColor * directionalLight.color * cos * directionalLight.intensity;
    outputColor.w = color.w * textureColor.w;
    return outputColor;
}


PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    
    float4 color = gMaterial.color;
    
    float4 textureColor = gTexture[input.textureIndex].Sample(gSampler, transformedUV.xy);
    
    if (gDirectionalLight.enableLight)
    {
        if (gDirectionalLight.enableLight == 1)
        {
            output.color = LambertReflectance(input.normal, color, textureColor, gDirectionalLight);
        }
        else if (gDirectionalLight.enableLight == 2)
        {
            output.color = HalfLambert(input.normal, color, textureColor, gDirectionalLight);
        }
    }
    else
    {
        output.color = color * textureColor;
    }
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
