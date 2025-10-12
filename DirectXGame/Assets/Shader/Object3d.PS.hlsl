#include "Object3d.hlsli"

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
    float32_t shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    int32_t enableLight;
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);


float32_t4 HalfLambert(float32_t3 normal, float32_t4 color, float32_t4 textureColor, DirectionalLight directionalLight)
{
    float NdotL = dot(normalize(normal), -gDirectionalLight.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float32_t4 outputColor = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    outputColor.w = color.w * textureColor.w;
    return outputColor;
}

float32_t4 LambertReflectance(float32_t3 normal, float32_t4 color, float32_t4 textureColor, DirectionalLight directionalLight)
{
    float NdotL = dot(normalize(normal), -directionalLight.direction);
    float cos = saturate(dot(normalize(normal), -directionalLight.direction));
    float32_t4 outputColor = color * textureColor * directionalLight.color * cos * directionalLight.intensity;
    outputColor.w = color.w * textureColor.w;
    return outputColor;
}


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 transformedUV = mul(float32_t4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float32_t4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    if (gDirectionalLight.enableLight)
    {
        if (gDirectionalLight.enableLight == 1)
        {
            output.color = LambertReflectance(input.normal, gMaterial.color, textureColor, gDirectionalLight);
        }
        else if (gDirectionalLight.enableLight == 2)
        {
            output.color = HalfLambert(input.normal, gMaterial.color, textureColor, gDirectionalLight);
        }
    }
    else
    {
        output.color = gMaterial.color * textureColor;
    }
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
