#include "PostEffect.hlsli"

// 常時走査線パラメータ用の定数バッファ
cbuffer ConstantScanlineParams : register(b0)
{
    float4x4 padding;
    float4x3 padding1;
    float intensity;      // 走査線の強度 (0.0 - 1.0)
    float speed;          // 走査線の移動速度
    float lineWidth;      // 走査線の幅
    float time;           // 時間パラメータ
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    // 元の色をサンプリング
    float4 baseColor = gTexture.Sample(gSampler, input.texcoord);
    
    float2 uv = input.texcoord;
    
    // === 1. 細かい走査線（静的、画面全体）===
    float fineScanline = sin(uv.y * 720.0 * 2.0) * 0.5 + 0.5;
    fineScanline = pow(fineScanline, 20.0); // 細い線にする
    
    // === 2. 非常に控えめなグロー効果（ゆっくり移動）===
    float glowY = frac(uv.y + time * speed * 0.15);
    float glowScanline = exp(-pow((glowY - 0.5) * 30.0, 2.0)); // より細いガウス分布
    
    // === 3. 縦方向のスキャンライン（微細）===
    float verticalScanline = sin(uv.x * 1280.0 * 0.5) * 0.5 + 0.5;
    verticalScanline = pow(verticalScanline, 30.0); // 非常に細い線
    
    // === 合成（すべて控えめに）===
    float scanlineEffect = 0.0;
    
    // 細かい走査線（控えめ）
    scanlineEffect += fineScanline * 0.08;
    
    // グロー走査線（非常に柔らかい光）
    scanlineEffect += glowScanline * 0.06;
    
    // 縦方向のスキャンライン（非常に控えめ）
    scanlineEffect += verticalScanline * 0.03;
    
    // 強度を適用
    scanlineEffect *= intensity;
    
    // === カラー調整 ===
    // サイバーパンクらしい青みがかった走査線（より暗めに）
    float3 scanlineColor = float3(0.3, 0.5, 0.8); // 控えめな青白い色
    
    // 基本色に走査線を加算
    output.color.rgb = baseColor.rgb + scanlineColor * scanlineEffect;
    output.color.a = baseColor.a;
    
    // === 微細なビネット（周辺を少し暗く）===
    float2 center = uv - 0.5;
    float vignette = 1.0 - dot(center, center) * 0.1;
    output.color.rgb *= vignette;
    
    output.color = saturate(output.color);
    
    return output;
}
