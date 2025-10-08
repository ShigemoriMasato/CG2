#include "Particle.hlsli"
#include "ShaderCommon.hlsli"

struct VertexData
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

StructuredBuffer<ParticleData> gTransformMatrix : register(t0);

ParticleVSOutput main(VertexData input, uint instanceId : SV_InstanceID)
{
    ParticleVSOutput output;
    output.position = mul(input.position, gTransformMatrix[instanceId].wvp);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix[instanceId].world));
    output.color = gTransformMatrix[instanceId].color;
    return output;
}
