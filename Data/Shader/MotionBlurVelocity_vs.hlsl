cbuffer CbMotionBlur : register(b0)
{
    row_major float4x4 currentWorldViewProjection;
    row_major float4x4 previousWorldViewProjection;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 currentClip : TEXCOORD0;
    float4 previousClip : TEXCOORD1;
};


VS_OUT main(float3 position : POSITION)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 localPosition = float4(position, 1.0f);
    
    output.currentClip = mul(localPosition, currentWorldViewProjection);
    output.previousClip = mul(localPosition, previousWorldViewProjection);
    output.position = output.currentClip;
    
    return output;

}