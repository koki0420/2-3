Texture2D trailTex : register(t0);
SamplerState trailSampler : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR;
};

float4 main(VS_OUT pin) : SV_TARGET
{

    float4 texColor =
        trailTex.Sample(
            trailSampler,
            pin.texcoord);

    return texColor * pin.color*3.0f;
}