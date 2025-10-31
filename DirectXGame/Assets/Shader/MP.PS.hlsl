#include "MP.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

cbuffer TileMapParams : register(b0)
{
    float2 gMapWorldSize;
    float4 gWaveColor;
};

Texture2D<float4> gTextures[8] : register(t1); // t0..t7 をまとめて使う
Texture2D<float4> gDiffuseMap : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
   
    float4 textureColor = gTextures[input.textureIndex].Sample(gSampler, input.texcoord);
    float4 baseColor = textureColor * input.color;
  
    float2 worldXY = input.worldPos.xy;
    float2 uvTile = worldXY / gMapWorldSize;
    uvTile = saturate(uvTile);

    float4 tileState = gDiffuseMap.SampleLevel(gSampler, uvTile, 0);

    float3 waveColor = tileState.rgb;
    float intensity = saturate(max(tileState.a, max(tileState.r, max(tileState.g, tileState.b))));

    float4 colorWithWave = baseColor;
    colorWithWave.rgb = lerp(baseColor.rgb, waveColor, intensity);

    float4 finalColor = lerp(colorWithWave, gWaveColor, intensity * 0.25f);

    output.color = saturate(finalColor);
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
