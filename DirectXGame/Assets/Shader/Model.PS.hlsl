#include "Model.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
    float shininess;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture[1024] : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    float4 transformedUV = mul(float4(input.texcoord, 0.0f, 1.0f), gMaterial.uvTransform);
    float4 textureColor = gTexture[0].Sample(gSampler, input.texcoord);
    
    PixelShaderOutput output;
    output.color = gMaterial.color * textureColor;
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
