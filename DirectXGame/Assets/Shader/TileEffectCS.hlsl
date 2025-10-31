cbuffer WaveSource : register(b0)
{
    int2 gWavePos;
    float gRadius;
    float gIntensity;
    float gFalloff;
    int2 gMapSize;
    float gTime;
    float gWaveStartTime;
    float gWaveActiveDuration;
};

Texture2D<float4> gPrevState : register(t0);
RWTexture2D<float4> gNextState : register(u0);
[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    int2 pos = int2(id.xy);
    if (pos.x >= gMapSize.x || pos.y >= gMapSize.y)
        return;

    float4 prev = gPrevState.Load(int3(pos, 0));
    float4 sum = float4(0, 0, 0, 0);
    int count = 0;

    for (int oy = -1; oy <= 1; ++oy)
    {
        for (int ox = -1; ox <= 1; ++ox)
        {
            if (ox == 0 && oy == 0)
                continue;
            int2 n = pos + int2(ox, oy);
            if (n.x < 0 || n.y < 0 || n.x >= gMapSize.x || n.y >= gMapSize.y)
                continue;

            sum += gPrevState.Load(int3(n, 0));
            ++count;
        }
    }

    float4 neighborAvg = sum / max(count, 1);
    float4 diffusion = neighborAvg - prev;
    float diffusionStrength = 0.2;
    float4 next = prev + diffusion * diffusionStrength;

    if (gRadius > 0 && gIntensity > 0)
    {
        float2 d = (float2) pos - (float2) gWavePos;
        float dist = length(d);

        float k = 6.0 / max(1.0, gRadius);
        float omega = 6.0;
        float wave = sin(dist * k - (gTime - gWaveStartTime) * omega);

        float rim = saturate(dist / gRadius);
        float atten = exp(-gFalloff * dist) * smoothstep(1.0, 0.0, rim);

        float wavePulse = max(0.0, wave);
        float influence = wavePulse * atten * gIntensity;

        float3 centerColor = float3(0.05f, 0.3f, 1.0f);
        float3 outerColor = float3(0.6f, 0.8f, 1.0f);
        float nd = saturate(dist / max(1.0f, gRadius));
        float3 waveColor = lerp(centerColor, outerColor, nd);

        float lifeTime = saturate((gTime - gWaveStartTime) / gWaveActiveDuration);
        float fade = 1.0 - lifeTime;
        if (fade <= 0.001f)
            fade = 0.0;

        next.rgb += waveColor * influence * fade;
        next.a = saturate(next.a + influence * 0.5 * fade);
    }

    next *= 0.98;

    if (next.a < 0.01)
        next = float4(0, 0, 0, 0);

    gNextState[pos] = saturate(next);
}