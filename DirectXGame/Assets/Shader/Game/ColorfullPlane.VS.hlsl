#include "ColorfullPlane.hlsli"

struct VSInput {
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

cbuffer WVP : register(b0)
{
    float4x4 wvp;
}

float4 ConvertColor(uint color)
{
    float4 result = float4(
        ((color >> 24) & 0xFF) / 255.0,
        ((color >> 16) & 0xFF) / 255.0,
        ((color >> 8) & 0xFF) / 255.0,
        (color & 0xFF) / 255.0);
    
    return result;
}

VSOutput main(VSInput input) {
    VSOutput output;
    output.position = mul(input.position, wvp);
    output.texcoord = input.texcoord;
    output.color = input.color;
    return output;
}
