#include "PostEffect.hlsli"

struct PostEffectInfo
{
    float4 color = { };
    float value1 = 0.0f;
    float value2 = 0.0f;
    float value3 = 0.0f;
    int enablePostEffect = 0;
};

StructuredBuffer<PostEffectInfo> info : register(b0);

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    if (info[0].enablePostEffect)
    {
        output.color = Blur(gSampler, gTexture, input, info[0].value1, info[0].value2);
    }
    
    if (info[1].enablePostEffect)
    {
        output.color = Grayscale(output.color);
    }

    
    if (output.color.w <= 0.01f)
    {
        discard;
    }
    
    return output;
}
