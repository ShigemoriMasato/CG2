#include "PostEffect.hlsli"

cbuffer FadeParameters : register(b0)
{
    float4 padding;
    float FadeAmount; // フェードの進行度 (0.0 = no fade, 1.0 = full fade)
    float FadeType; // フェードタイプ (0=Black, 1=White, 2=Radial, 3=Wipe, 4=Dissolve)
    float2 padding1;
    
    float3 FadeColor; // フェードカラー
}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ノイズ関数（ディゾルブ用）
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

// ブラックフェード
float3 BlackFade(float3 color, float amount)
{
    return lerp(color, float3(0.0, 0.0, 0.0), amount);
}

// ホワイトフェード
float3 WhiteFade(float3 color, float amount)
{
    return lerp(color, float3(1.0, 1.0, 1.0), amount);
}

// カスタムカラーフェード
float3 ColorFade(float3 color, float3 fadeColor, float amount)
{
    return lerp(color, fadeColor, amount);
}

// 放射状フェード（中心から外側へ）
float3 RadialFade(float3 color, float2 uv, float amount)
{
    float2 center = float2(0.5, 0.5);
    float dist = length(uv - center);
    
    // 距離を正規化（0.0 - 1.0）
    float normalizedDist = dist / 0.707; // sqrt(0.5^2 + 0.5^2) = 0.707
    
    // フェードの進行度に応じて閾値を調整
    float threshold = amount * 1.2; // 1.2倍で完全にフェード
    
    // スムースステップで滑らかな境界
    float fadeFactor = smoothstep(threshold - 0.1, threshold + 0.1, normalizedDist);
    
    return lerp(color, float3(0.0, 0.0, 0.0), 1.0 - fadeFactor);
}

// ワイプフェード（左から右へ）
float3 WipeFade(float3 color, float2 uv, float amount)
{
    // 斜めワイプ（左下から右上）
    float wipePos = uv.x * 0.7 + uv.y * 0.3;
    
    // フェードの進行度に応じて閾値を調整
    float threshold = amount * 1.2;
    
    // スムースステップで滑らかなエッジ
    float fadeFactor = smoothstep(threshold - 0.15, threshold + 0.05, wipePos);
    
    return lerp(float3(0.0, 0.0, 0.0), color, fadeFactor);
}

// ディゾルブフェード（ランダムに消える）
float3 DissolveFade(float3 color, float2 uv, float amount, float3 fadeColor)
{
    // ノイズパターンを生成
    float noise = random(uv * 100.0);
    
    // フェードの進行度と比較
    float threshold = amount;
    
    // エッジの幅
    float edgeWidth = 0.1;
    
    // ディゾルブマスク
    float dissolveMask = smoothstep(threshold - edgeWidth, threshold, noise);
    
    // エッジの光彩効果
    float edgeGlow = smoothstep(threshold - edgeWidth, threshold, noise) -
   smoothstep(threshold, threshold + edgeWidth, noise);
    
    // フェードカラーに基づいたエッジカラー（白の場合は白く光る）
    float3 edgeColor = fadeColor * edgeGlow * 2.0;
    
    // 最終的な色を計算
    float3 finalColor = lerp(fadeColor, color + edgeColor, dissolveMask);
    
    return finalColor;
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // テクスチャから色をサンプリング
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    float3 finalColor = textureColor.rgb;
    
    // フェードタイプに応じて処理を分岐
    int fadeTypeInt = int(FadeType);

    if (fadeTypeInt == 0)
    {
      // ブラックフェード
 finalColor = BlackFade(finalColor, FadeAmount);
    }
    else if (fadeTypeInt == 1)
    {
    // ホワイトフェード
        finalColor = WhiteFade(finalColor, FadeAmount);
    }
    else if (fadeTypeInt == 2)
    {
        // 放射状フェード
        finalColor = RadialFade(finalColor, input.texcoord, FadeAmount);
    }
    else if (fadeTypeInt == 3)
    {
        // ワイプフェード
        finalColor = WipeFade(finalColor, input.texcoord, FadeAmount);
    }
    else if (fadeTypeInt == 4)
    {
    // ディゾルブフェード（fadeColorを渡す）
        finalColor = DissolveFade(finalColor, input.texcoord, FadeAmount, FadeColor);
    }
    else
    {
        // カスタムカラーフェード（デフォルト）
        finalColor = ColorFade(finalColor, FadeColor, FadeAmount);
    }
    
    // アルファ値は保持
    output.color = float4(finalColor, textureColor.a);
    
    return output;
}