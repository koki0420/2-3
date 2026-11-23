Texture2D SceneTexture : register(t0);
Texture2D VelocityTexture : register(t1);

SamplerState LinerSampler : register(s0);

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
    float4 color = 0.0f;
    
    float2 velocity = VelocityTexture.Sample(LinerSampler, input.texcoord).xy;
    velocity *= blurStrength;
    
    //ë¨ìxÇ™ëÂÇ´Ç∑Ç¨ÇÈèÍçáÇÕêßå¿
    velocity = clamp(velocity, -0.05f, 0.05f);
    
    const int MAX_SAMPLES = 8;
    
    float totalWeight = 0.0f;
    
    
    for (int i = 0; i < MAX_SAMPLES;++i)
    {
        if (i >= sampleCount) break;
        
        float t = (float) i / max(sampleCount - 1, 1);
        
        float2 uv = input.texcoord - velocity * t;
        float weight = 1.0f - t*0.5f;
        
        color += SceneTexture.Sample(LinerSampler, uv) * weight;
        
        totalWeight += weight;

    }
    
    return color / max(totalWeight, 0.0001f);

}