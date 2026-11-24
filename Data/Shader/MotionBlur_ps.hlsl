Texture2D SceneTexture : register(t0);
Texture2D VelocityTexture : register(t1);

SamplerState LinearSampler : register(s0);

cbuffer CbBlur : register(b0)
{
    float blurStrength;
    float2 texelSize;
    int sampleCount;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
    float2 velocity =
        VelocityTexture.Sample( LinearSampler,input.texcoord).xy;

    velocity *= blurStrength;

    velocity = clamp(velocity,-0.05f,0.05f);

    const int MAX_SAMPLES = 8;

    int count = clamp(sampleCount,1,MAX_SAMPLES);

    float4 color = 0.0f;
    float totalWeight = 0.0f;

    for (int i = 0; i < MAX_SAMPLES; ++i)
    {
        if (i >= count)
            break;

        float t = (float) i / max(count - 1, 1);

        float2 uv =
            input.texcoord - velocity * t;

        uv = clamp(
            uv,
            float2(0.0f, 0.0f),
            float2(1.0f, 1.0f));

        float weight = 1.0f - t * 0.5f;

        color += SceneTexture.Sample(
            LinearSampler,
            uv) * weight;

        totalWeight += weight;
    }

    return color / max(totalWeight, 0.0001f);
}