#include "BackGround.hlsli"

struct VSInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct ParticleData
{
    float4x4 wvp;
    float4x4 world;
    float4 color;
};

StructuredBuffer<ParticleData> data : register(t0);

VSOutput main(VSInput input, uint instance : SV_InstanceID)
{
    VSOutput output;
    output.position = mul(input.position, data[instance].wvp);
    output.texCoord = input.texcoord;
    output.color = data[instance].color;
    output.normal = normalize(mul(input.normal, (float3x3) data[instance].world));
    return output;
}
