#include "Particle.hlsli"

struct ParticleData
{
    float4x4 VP;
    float4x4 World;
    uint color;
    uint textureIndex;
    float2 padding;
    float4 pad;
};

StructuredBuffer<ParticleData> gTransformMatrix : register(t0);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

float4 ConvertColor(uint color)
{
    float4 result = float4(
        ((color >> 24) & 0xFF) / 255.0,
        ((color >> 16) & 0xFF) / 255.0,
        ((color >> 8) & 0xFF) / 255.0,
        (color & 0xFF) / 255.0);
    
    return result;
}

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gTransformMatrix[instanceId].World, gTransformMatrix[instanceId].VP));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix[instanceId].World));
    output.color = ConvertColor(gTransformMatrix[instanceId].color);
    output.textureIndex = gTransformMatrix[instanceId].textureIndex;
    return output;
}
