#include "Particle.hlsli"
#include "ShaderCommon.hlsli"

StructuredBuffer<ParticleData> gTransformMatrix : register(t0);

ParticleVSOutput main(VertexData input, uint instanceId : SV_InstanceID)
{
    ParticleVSOutput output;
    output.position = mul(input.position, gTransformMatrix[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix[instanceId].World));
    output.color = gTransformMatrix[instanceId].Color;
    return output;
}
