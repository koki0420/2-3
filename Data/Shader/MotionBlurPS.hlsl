Texture2D currentTexture : register(t0);
Texture2D previousTexture : register(t1);

SamplerState samplerState : register(s0);

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

float main(VS_OUT pin) : SV_TARGET
{
    float4 current = currentTexture.Sample(samplerState, pin.texcoord);
    float4 previous = previousTexture.Sample(samplerState, pin.texcoord);
    
    return current * 0.85f + previous * 0.15f;

}