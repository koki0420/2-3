struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 currentClip : TEXCOORD0;
    float4 previousClip : TEXCOORD1;
};

float2 main(VS_OUT input) : SV_TARGET
{
    float2 currentNdc = input.currentClip.xy / input.currentClip.w;
    
    float2 previousNdc = input.previousClip.xy / input.previousClip.w;
    
    float2 velocity = currentNdc - previousNdc;
    
    //NDC‚©‚çUV‚Ö‚Ì•ÏŠ·
    velocity.y = -velocity.y;
    
    velocity *= 0.5f;
    
    return velocity;
}