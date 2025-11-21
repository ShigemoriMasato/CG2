#include "WaterObj.hlsli"

cbuffer Info : register(b0)
{
    float4 color;
    float waterHeight;
    float3 padding;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture[] : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    //parameters
    const float kBaseRatio = 1.0f;
    const float kBaseAlpha = 0.8f;
    const float kDepth = 3.0f;
    //parameters
    PixelShaderOutput output;
    float4 baseColor = gTexture[input.textureIndex].Sample(gSampler, input.texcoord) * color;
    
    if (input.worldPos.y < waterHeight)
    {
        float4 underwaterColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
        
        output.color = baseColor * kBaseRatio + underwaterColor * (1.0f - kBaseRatio);
        
        float t = clamp((waterHeight - input.worldPos.y) / kDepth, 0.0f, 1.0f);
        output.color.a = lerp(kBaseAlpha, 0.05f, t);

        return output;
    }
    
    output.color = baseColor;
    
    return output;
}
