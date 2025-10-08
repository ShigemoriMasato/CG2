
struct VertexData
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct MatrixData
{
    float4x4 WVP;
    float4x4 World;
};

struct Material
{
    float4 color;
    float4x4 uvTransform;
    float shininess;
};

struct DirectionalLightData
{
    int enableLight;
    float3 padding;
    float4 color;
    float3 direction;
    float intensity;
};

struct Camera
{
    float3 worldPosition;
};
