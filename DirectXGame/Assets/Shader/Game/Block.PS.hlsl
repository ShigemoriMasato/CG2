#include "Block.hlsli"

struct PSOutput
{
    float4 color : SV_Target0;
};

PSOutput main(PSInput input)
{
    PSOutput output;
    
    if (input.color.a < 0.1f)
    {
        discard;
    }
    
    float outlineWidth = 0.05f;
    
    output.color = input.color;
    
    if (input.texCoord.x < outlineWidth || input.texCoord.x > 1.0f - outlineWidth ||
       input.texCoord.y < outlineWidth || input.texCoord.y > 1.0f - outlineWidth)
    {
        output.color = input.outlineColor;
    }
    
    return output;
}
