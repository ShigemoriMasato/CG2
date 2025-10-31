struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
    int textureIndex : TEXCOORD1;
    float3 worldPos : TEXCOORD2;
};