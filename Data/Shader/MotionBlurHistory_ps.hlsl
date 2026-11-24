Texture2D CurrentBlur : register(t0);
Texture2D PreviousHistory : register(t1);
SamplerState LinearSampler : register(s0);

cbuffer CbHistory : register(b0)
{
    float decay;
    float3 padding;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
    float4 current = CurrentBlur.Sample(LinearSampler, input.texcoord);
    float4 previous = PreviousHistory.Sample(LinearSampler, input.texcoord);

    float currentAlpha = saturate(current.a);
    float oldAlpha = saturate(previous.a * decay);
    float alpha = currentAlpha + oldAlpha * (1.0f - currentAlpha);

    float3 rgb =
        current.rgb * currentAlpha +
        previous.rgb * oldAlpha * (1.0f - currentAlpha);

    return float4(rgb / max(alpha, 0.0001f), alpha);
}