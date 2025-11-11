#include "PostEffect.hlsli"

cbuffer SlowMotionParameters : register(b0)
{
    float4x4 padding;
    float ChromaticAberration;  // 色収差強度 (0.0 - 1.0)
    float VignetteStrength;     // ビネット強度 (0.0 - 1.0)
    float Saturation;           // 彩度 (0.0=モノクロ, 1.0=通常)
    float Intensity;            // 全体の強度 (0.0 - 1.0)
}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// RGBをグレースケールに変換
float3 ToGrayscale(float3 color)
{
    float gray = dot(color, float3(0.299, 0.587, 0.114));
    return float3(gray, gray, gray);
}

// ビネット効果（より強力に）
float GetVignette(float2 uv)
{
    // 画面中央からの距離を計算
    float2 center = float2(0.5, 0.5);
    float2 diff = uv - center;
    
    // 距離を0-1の範囲に正規化
    float dist = length(diff);
    
    // ビネット効果を計算（中心=1.0、端=0.0）
    // より急激なビネット効果のために指数を使用
    float vignette = 1.0 - smoothstep(0.2, 1.0, dist);
    vignette = pow(vignette, 1.5);  // より強い効果
    
    return vignette;
}

// クロマティックアベレーション（色収差）- より強力に
float3 ChromaticAberrationEffect(float2 uv, float strength)
{
    // 画面中央からの方向ベクトル
    float2 center = float2(0.5, 0.5);
    float2 direction = uv - center;
    
    // 距離に応じてオフセットを強化
    float dist = length(direction);
    float distStrength = pow(dist, 1.5);
    
    // 各チャンネルで異なるオフセット（より大きく）
    float offset = strength * 0.015 * (1.0 + distStrength);
    
    // UV座標を計算し、0.0〜1.0の範囲にクランプ
    float2 uvR = saturate(uv + direction * offset * 1.2);
    float2 uvG = saturate(uv);
    float2 uvB = saturate(uv - direction * offset * 1.2);
    
    float r = gTexture.Sample(gSampler, uvR).r;
    float g = gTexture.Sample(gSampler, uvG).g;
    float b = gTexture.Sample(gSampler, uvB).b;
    
    return float3(r, g, b);
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    float2 uv = input.texcoord;
    
    // クロマティックアベレーション適用
    float3 color;
    if (ChromaticAberration > 0.01)
    {
        color = ChromaticAberrationEffect(uv, ChromaticAberration * Intensity);
    }
    else
    {
        color = gTexture.Sample(gSampler, uv).rgb;
    }
    
    // 彩度調整（より強く）
    if (Saturation < 0.99)
    {
        float3 gray = ToGrayscale(color);
        // 彩度の変化をより顕著に
        float adjustedSat = pow(Saturation, 0.8);
        color = lerp(gray, color, adjustedSat);
    }
    
    // ビネット効果適用（より強く）
    if (VignetteStrength > 0.01)
    {
        float vignette = GetVignette(uv);
        float vignetteEffect = lerp(1.0, vignette, VignetteStrength * Intensity);
        
        // ビネット部分をより暗く
        vignetteEffect = pow(vignetteEffect, 1.3);
        color *= vignetteEffect;
    }
    
    // エッジグロー効果（スローモーション感を強調）
    if (Intensity > 0.5)
    {
        float2 center = float2(0.5, 0.5);
        float dist = length(uv - center);
        
        // 画面端で微かに青みを帯びる
        float edgeGlow = smoothstep(0.5, 1.0, dist) * (Intensity - 0.5) * 0.3;
        color += float3(0.1, 0.15, 0.3) * edgeGlow;
    }
    
    output.color = float4(color, 1.0);
    
    return output;
}
