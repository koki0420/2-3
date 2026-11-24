Texture2D VelocityTexture : register(t0);

SamplerState LinearSampler : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
    float2 velocity =
        VelocityTexture.Sample(
            LinearSampler,
            input.texcoord).xy;

    // ‘¬“x‚ğF‚Æ‚µ‚Ä‰Â‹‰»
    float2 visual = velocity * 20.0f + 0.5f;

    return float4(visual, 0.0f, 1.0f);
}