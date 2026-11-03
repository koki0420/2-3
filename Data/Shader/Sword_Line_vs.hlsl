#include "Scene.hlsli"

struct VS_IN
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    

};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    
    vout.position = mul(float4(vin.position, 1.0f), viewProjection);
    vout.texcoord = vin.texcoord;
    vout.color = vin.color;
    
    return vout;
}