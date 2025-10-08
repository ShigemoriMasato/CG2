#include "Particle.hlsli"
#include "ShaderCommon.hlsli"

Texture2D<float4> gTexture : register(t0);

SamplerState gSampler : register(s0);

PixelShaderOutput main(ParticleVSOutput input)
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
