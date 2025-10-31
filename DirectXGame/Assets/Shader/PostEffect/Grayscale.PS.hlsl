#include "PostEffect.hlsli"

cbuffer GrayscaleParameters : register(b0)
{
    float2 padding;
    float intensity; // グレースケールの強度（0.0〜1.0）
}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    float clampedIntensity = clamp(intensity, 0.0, 1.0);
    
    // 元の色をサンプリング
    float4 color = gTexture.Sample(gSampler, input.texcoord);

    // グレースケール変換（人間の視覚に基づく加重平均）
    float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));

    // グレースケール色を作成
    float3 grayscale = float3(luminance, luminance, luminance);

    // 強度で補間（カラーとグレースケールのブレンド）
    float3 finalColor = lerp(color.rgb, grayscale, clampedIntensity);

    // 出力
    output.color = float4(finalColor, 1.0);
    return output;
}
