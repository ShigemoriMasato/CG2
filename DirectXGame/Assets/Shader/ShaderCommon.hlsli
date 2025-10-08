
struct MatrixData
{
    float4x4 world;
    float4x4 wvp;
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

struct CameraData
{
    float3 worldPosition;
};

struct ParticleData
{
    float4x4 world;
    float4x4 wvp;
    float4 color;
};
