#include "Particle.hlsli"

struct ParticleData
{
    float4 color;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
StructuredBuffer<ParticleData> gData : register(t1);

SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = input.color * textureColor;
    
    if (output.color.w < 0.02f)
    {
        discard;
    }
    
    return output;
}
