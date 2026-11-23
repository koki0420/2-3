Texture2D BlurTexture : register(t0);

SamplerState LinearSampler : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
    return BlurTexture.Sample(
        LinearSampler, input.texcoord);

}