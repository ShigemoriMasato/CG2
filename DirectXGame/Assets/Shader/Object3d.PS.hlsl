#include "ShaderCommon.hlsli"

ConstantBuffer<Material> gMaterial : register(b0);

ConstantBuffer<DirectionalLightData> gDirectionalLight : register(b1);

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);


float4 HalfLambert(float3 normal, float4 color, float4 textureColor, DirectionalLightData directionalLight)
{
    float NdotL = dot(normalize(normal), -gDirectionalLight.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float4 outputColor = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
    outputColor.w = color.w * textureColor.w;
    return outputColor;
}

float4 LambertReflectance(float3 normal, float4 color, float4 textureColor, DirectionalLightData directionalLight)
{
    float NdotL = dot(normalize(normal), -directionalLight.direction);
    float cos = saturate(dot(normalize(normal), -directionalLight.direction));
    float4 outputColor = color * textureColor * directionalLight.color * cos * directionalLight.intensity;
    outputColor.w = color.w * textureColor.w;
    return outputColor;
}


PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
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
