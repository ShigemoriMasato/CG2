#include "Particle.hlsli"

struct ParticleData
{
    float4x4 VP;
    float4x4 World;
    float4 Color;
    uint textureIndex;
    float3 padding;
};

StructuredBuffer<ParticleData> gTransformMatrix : register(t0);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gTransformMatrix[instanceId].World, gTransformMatrix[instanceId].VP));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix[instanceId].World));
    output.color = gTransformMatrix[instanceId].Color;
    output.textureIndex = gTransformMatrix[instanceId].textureIndex;
    return output;
}
