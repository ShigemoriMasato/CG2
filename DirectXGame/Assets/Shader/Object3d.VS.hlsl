#include "ShaderCommon.hlsli"

ConstantBuffer<MatrixData> gTransformMatrix : register(b0);

VertexShaderOutput main(VertexData input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, gTransformMatrix.WVP);
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float3x3)gTransformMatrix.World));
    return output;
}
