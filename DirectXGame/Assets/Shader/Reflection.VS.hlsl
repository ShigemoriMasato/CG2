#include "Reflection.hlsli"
#include "ShaderCommon.hlsli"

ConstantBuffer<MatrixData> gTransformMatrix : register(b0);

struct VertexData
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

ReflectionVSOutput main(VertexData input)
{
    ReflectionVSOutput output;
    output.position = mul(input.position, gTransformMatrix.wvp);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix.world));
    output.worldPosition = mul(input.position, gTransformMatrix.world).xyz;
    return output;
}
