#include "PostEffect.hlsli"

cbuffer GridTransitionParameters : register(b0)
{
    float Progress;      // 遷移の進行度 (0.0 - 1.0)
    float GridSize;      // グリッドサイズ（1辺のタイル数）
    float FadeColor;     // フェード色 (0=黒, 1=白)
    float Pattern;       // パターン (0=波紋状, 1=ランダム, 2=左から右)
}

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ノイズ関数（ランダムパターン用）
float random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

// グリッドタイル分解遷移
float3 GridTransitionEffect(float3 color, float2 uv, float progress)
{
    // グリッドの座標を計算
    float2 gridCoord = floor(uv * GridSize);
    float2 gridUV = gridCoord / GridSize;
    
    float delay = 0.0;
    int patternType = int(Pattern);
    
    if (patternType == 0)
    {
        // 波紋状パターン（外側から中心へ）
        float2 center = float2(0.5, 0.5);
        float2 gridCenter = (gridCoord + 0.5) / GridSize;
        float dist = length(gridCenter - center);
        
        // 最大距離を計算（コーナーからの距離）
        float maxDist = length(float2(0.5, 0.5));
        
        // 距離を反転（外側=0.0、中心=0.8）
        delay = (1.0 - dist / maxDist) * 0.8;
    }
    else if (patternType == 1)
    {
        // ランダムパターン
        delay = random(gridCoord) * 0.8;
    }
    else if (patternType == 2)
    {
        // 左から右へ
        delay = gridUV.x * 0.8;
    }
    else if (patternType == 3)
    {
        // 上から下へ
        delay = gridUV.y * 0.8;
    }
    else if (patternType == 4)
    {
        // チェッカーボードパターン
        float checker = fmod(gridCoord.x + gridCoord.y, 2.0);
        delay = checker * 0.4 + (gridUV.x + gridUV.y) * 0.2;
    }
    
    // 進行度に応じてタイルが消える
    float tileProgress = saturate((progress * 1.5 - delay) / 0.5);
    
    // フェード色を決定
    float3 targetColor = FadeColor > 0.5 ? float3(1.0, 1.0, 1.0) : float3(0.0, 0.0, 0.0);
    
    // タイルが消えるアニメーション
    // エッジの光る効果を追加
    float edgeGlow = 0.0;
    if (tileProgress > 0.0 && tileProgress < 1.0)
    {
        // タイルの境界で光る
        float2 gridLocalUV = frac(uv * GridSize);
        float edgeDist = min(min(gridLocalUV.x, 1.0 - gridLocalUV.x),
                             min(gridLocalUV.y, 1.0 - gridLocalUV.y));
        
        // 境界部分で光らせる
        if (edgeDist < 0.15)
        {
            edgeGlow = (1.0 - edgeDist / 0.15) * (1.0 - abs(tileProgress - 0.5) * 2.0) * 0.5;
        }
    }
    
    // 最終的な色を計算
    float3 glowColor = FadeColor > 0.5 ? float3(1.0, 1.0, 0.8) : float3(0.3, 0.6, 1.0);
    float3 finalColor = lerp(color + glowColor * edgeGlow, targetColor, tileProgress);
    
    return finalColor;
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // テクスチャから色をサンプリング
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // グリッドタイル分解遷移を適用
    float3 finalColor = GridTransitionEffect(textureColor.rgb, input.texcoord, Progress);
    
    // アルファ値は保持
    output.color = float4(finalColor, textureColor.a);
    
    return output;
}
