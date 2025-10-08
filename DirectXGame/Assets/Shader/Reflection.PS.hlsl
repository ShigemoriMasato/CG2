#include "Reflection.hlsli"
#include "ShaderCommon.hlsli"

//Material
ConstantBuffer<Material> gMaterial : register(b0);

//Light
ConstantBuffer<DirectionalLightData> gDirectionalLight : register(b1);

//Camera`s WorldPosition
ConstantBuffer<Camera> gCamera : register(b2);

//Texture
Texture2D<float4> gTexture : register(t0);

//Sampler
SamplerState gSampler : register(s0);


float3 HalfLambert(float3 normal, float4 color, float4 textureColor, DirectionalLightData directionalLight)
{
    float NdotL = dot(normalize(normal), -gDirectionalLight.direction);
    float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
    float3 outputColor = (gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity).rgb;
    return outputColor;
}

float3 LambertReflectance(float3 normal, float4 color, float4 textureColor, DirectionalLightData directionalLight)
{
    float NdotL = dot(normalize(normal), -directionalLight.direction);
    float cos = saturate(dot(normalize(normal), -directionalLight.direction));
    float3 outputColor = (color * textureColor * directionalLight.color * cos * directionalLight.intensity).rgb;
    return outputColor;
}


PixelShaderOutput main(ReflectionVSOutput input)
{
    PixelShaderOutput output;
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture.Sample(gSampler, transformedUV.xy);
    
    if (gDirectionalLight.enableLight)
    {
        //鏡面反射の計算
        float3 toEye = normalize(gCamera.worldPosition - input.worldPosition);
        float3 reflectLight = reflect(gDirectionalLight.direction, normalize(input.normal));
        
        float RdotE = dot(reflectLight, toEye);
        float specularPow = pow(saturate(RdotE), gMaterial.shininess);
        
        float3 diffuse;
        
        if (gDirectionalLight.enableLight == 1)
        {
            diffuse = LambertReflectance(input.normal, gMaterial.color, textureColor, gDirectionalLight);
        }
        else if (gDirectionalLight.enableLight == 2)
        {
            diffuse = HalfLambert(input.normal, gMaterial.color, textureColor, gDirectionalLight);
        }
        
        float3 specular = gDirectionalLight.color.rgb * gDirectionalLight.intensity * specularPow;
        
        output.color.rgb = diffuse.rgb + specular;
        output.color.a = gMaterial.color.a * textureColor.a;
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
