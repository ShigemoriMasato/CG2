#include "Reflection.hlsli"
#include "ShaderCommon.hlsli"

ConstantBuffer<MatrixData> gTransformMatrix : register(b0);

ReflectionVSOutput main(VertexData input)
{
    ReflectionVSOutput output;
    output.position = mul(input.position, gTransformMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix.World));
    output.worldPosition = mul(input.position, gTransformMatrix.World).xyz;
    return output;
}
