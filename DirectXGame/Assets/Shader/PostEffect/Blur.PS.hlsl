#include "PostEffect.hlsli"

// ブラーパラメータ用の定数バッファ
cbuffer BlurParams : register(b0)
{
    float intensity; // ブラー強度
    float kernelSize; // カーネルサイズ
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    if (kernelSize < 0.01f)
    {
        output.color = gTexture.Sample(gSampler, input.texcoord);
        return output;
    }

    float2 texelSize = 1.0f / float2(1280.0f, 720.0f);
    float sigma = kernelSize;
    int kernelRadius = int(ceil(sigma * 2.0f));

    float4 color = 0;
    float totalWeight = 0;

    for (int x = -kernelRadius; x <= kernelRadius; x++)
    {
        for (int y = -kernelRadius; y <= kernelRadius; y++)
        {
            float2 offset = float2(x, y) * texelSize;
            float distance = length(float2(x, y));
            float weight = exp(-0.5 * (distance * distance) / (sigma * sigma));

            float4 tex = gTexture.Sample(gSampler, input.texcoord + offset);

            // sRGB → Linear
            tex.rgb = pow(tex.rgb, 2.2f);

            color += tex * weight;
            totalWeight += weight;
        }
    }

    color /= totalWeight;

    // Linear → sRGB
    color.rgb = pow(color.rgb, 1.0f / 2.2f);

    float4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = lerp(originalColor, color, intensity);
    output.color = saturate(output.color);

    return output;
}