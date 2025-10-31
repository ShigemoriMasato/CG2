#include "MP.hlsli"

struct ModelParticleData
{
    float4x4 WVP;
    float4x4 World;
    float4 Color;
    int textureIndex;
    float3 padding;
};

StructuredBuffer<ModelParticleData> gTransformMatrix : register(t0);

struct VertexShaderInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint instanceId : SV_InstanceID) {
    VertexShaderOutput output;
    float4 worldPos = mul(input.position, gTransformMatrix[instanceId].World);
    output.position = mul(input.position, gTransformMatrix[instanceId].WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix[instanceId].World));
    output.color = gTransformMatrix[instanceId].Color;
    output.textureIndex = gTransformMatrix[instanceId].textureIndex;
    output.worldPos = worldPos.xyz;
    
    return output;
}
