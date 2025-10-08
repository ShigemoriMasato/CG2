#include "ShaderCommon.hlsli"

VertexShaderOutput main(VertexData input)
{
    VertexShaderOutput output;
    output.position = input.position;
    output.texcoord = input.texcoord;
    output.normal = input.normal;
    return output;
}
