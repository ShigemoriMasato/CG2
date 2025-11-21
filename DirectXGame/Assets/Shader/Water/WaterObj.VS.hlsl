#include "WaterObj.hlsli"

cbuffer MatrixData : register(b0) 
{
    float4x4 wvp;
    float4x4 world;
}

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint textureIndex : TEXTURE0;
    int nodeIndex : NODEINDEX0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, wvp);
    output.texcoord = input.texcoord;
    output.normal = mul(float4(input.normal, 1.0f), world).xyz;
    output.worldPos = mul(input.position, world).xyz;
    output.textureIndex = input.textureIndex;
    return output;
}
