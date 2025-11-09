struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    uint textureIndex : TEXTURE0;
};

#define PixelShaderInput VertexShaderOutput
