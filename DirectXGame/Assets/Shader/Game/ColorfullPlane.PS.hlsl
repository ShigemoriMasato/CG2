#include "ColorfullPlane.hlsli"

struct PSOutput
{
    float4 color : SV_TARGET0;
};

cbuffer Data : register(b0)
{
    float4 color;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    output.color = input.color * color;
    
    if (output.color.w < 0.01f)
    {
        discard;
    }
    
    return output;
}
