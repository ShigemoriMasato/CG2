#include "PostEffect.hlsli"

// グリッチパラメータ用の定数バッファ
cbuffer GlitchParams : register(b0)
{
    float4x4 padding;
    float4 padding1;
    float intensity;            // 全体の強度 (0.0 - 1.0)
    float rgbSplit;             // RGB色収差の強度 (0.0 - 1.0)
    float scanlineIntensity;    // スキャンライン強度 (0.0 - 1.0)
    float blockIntensity;       // ブロックノイズ強度 (0.0 - 1.0)
    float time;                 // 時間パラメータ
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

// ランダム関数（ハッシュベース）
float random(float2 st)
{
    return frac(sin(dot(st.xy, float2(12.9898, 78.233))) * 43758.5453123);
}

// ノイズ関数
float noise(float2 st)
{
    float2 i = floor(st);
    float2 f = frac(st);
    
    float a = random(i);
    float b = random(i + float2(1.0, 0.0));
    float c = random(i + float2(0.0, 1.0));
    float d = random(i + float2(1.0, 1.0));
    
    float2 u = f * f * (3.0 - 2.0 * f);
    
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // 強度が0の場合はそのまま返す
    if (intensity < 0.001f)
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
        return output;
    }
    
    float2 uv = input.texcoord;
    
    // === 1. RGB色収差エフェクト ===
    float splitAmount = rgbSplit * intensity * 0.02f;
    
    // 時間ベースのランダムなオフセット
    float randomOffset = (random(float2(time * 0.1, 0.0)) - 0.5) * 2.0;
    float horizontalShift = splitAmount * randomOffset;
    
    // 各色チャンネルを別々にサンプリング
    float r = gTexture.Sample(gSampler, uv + float2(horizontalShift, 0.0)).r;
    float g = gTexture.Sample(gSampler, uv).g;
    float b = gTexture.Sample(gSampler, uv - float2(horizontalShift, 0.0)).b;
    float a = gTexture.Sample(gSampler, uv).a;
    
    float4 color = float4(r, g, b, a);
    
    // === 2. スキャンライン（走査線）エフェクト ===
    if (scanlineIntensity > 0.001f)
    {
        // 高速に動く走査線
        float scanline = sin((uv.y + time * 0.5) * 600.0) * 0.5 + 0.5;
        scanline = pow(scanline, 10.0);
        
        // ランダムな水平ノイズバンド
        float noiseband = noise(float2(0.0, uv.y * 50.0 + time * 2.0));
        noiseband = step(0.95, noiseband);
        
        float scanlineEffect = (scanline * 0.3 + noiseband * 0.7) * scanlineIntensity * intensity;
        color.rgb *= (1.0 + scanlineEffect * 0.3);

    }
    
    // === 3. ブロックノイズエフェクト ===
    if (blockIntensity > 0.001f)
    {
        // ランダムな水平ブロック
        float blockSize = 8.0;
        float blockY = floor(uv.y * 720.0 / blockSize) * blockSize / 720.0;
        
        // 時間に基づくランダムなブロック出現
        float blockRandom = random(float2(blockY, floor(time * 10.0)));
        
        if (blockRandom > (1.0 - blockIntensity * intensity * 0.3))
        {
            // ブロックを水平にずらす
            float blockShift = (random(float2(blockY, floor(time * 5.0))) - 0.5) * 0.1;
            float2 blockUV = float2(uv.x + blockShift, uv.y);
            color = gTexture.Sample(gSampler, blockUV);
            
            // ブロックに色ノイズを追加
            float colorNoise = random(float2(blockY, time * 3.0));
            color.rgb += float3(colorNoise, colorNoise, colorNoise) * 0.1 * blockIntensity;
        }
    }
    
    // === 4. デジタルピクセル化エフェクト（強度が高い時） ===
    if (intensity > 0.5f)
    {
        float pixelSize = lerp(1.0, 4.0, (intensity - 0.5) * 2.0);
        float2 pixelatedUV = floor(uv * float2(1280.0, 720.0) / pixelSize) * pixelSize / float2(1280.0, 720.0);
        
        // ランダムな領域でのみピクセル化
        float pixelNoise = noise(float2(uv.y * 10.0, time));
        if (pixelNoise > 0.8)
        {
            color = gTexture.Sample(gSampler, pixelatedUV);
        }
    }
    
    // === 5. ランダムな明滅 ===
    float flicker = noise(float2(time * 10.0, 0.0));
    flicker = lerp(1.0, flicker, intensity * 0.1);
    color.rgb *= flicker;
    
    // === 6. 画面全体を暗くする（グリッチの雰囲気強化） ===
    // グリッチ発生時に画面を暗くし、不安定な雰囲気を演出
    float darkenAmount = intensity * 0.4f; // 最大40%暗くする
    color.rgb *= (1.0f - darkenAmount);
    
    // === 7. ビネット（グリッチ時により暗くする） ===
    float2 center = uv - 0.5;
    float vignette = 1.0 - dot(center, center) * intensity * 0.8; // 強度を上げた
    color.rgb *= vignette;
    
    // === 8. ランダムな暗部のフラッシュ（より不気味に） ===
    float darkFlash = noise(float2(time * 15.0, uv.x * 10.0));
    darkFlash = step(0.97, darkFlash); // まれに発生
    color.rgb *= lerp(1.0, 0.3, darkFlash * intensity);
    
    output.color = saturate(color);
    
    return output;
}
