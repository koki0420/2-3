#include "Scene.hlsli"

struct VS_OUT
{
	float4 vertex	: SV_POSITION;
	float2 texcoord	: TEXCOORD0;
	float3 normal	: NORMAL;
	float3 position : POSITION;
	float3 tangent	: TANGENT;
    
    float4 shadow_position : TEXCOORD5;
};

struct PointLight
{
    float4 position;
    float4 color;
    float range;
    float3 padding;
};


cbuffer CbLight : register(b2)
{
    float4 ambient_color;
    float4 directional_light_direction;
    float4 directional_light_color;
    
    
  


    PointLight point_light[19];
};


