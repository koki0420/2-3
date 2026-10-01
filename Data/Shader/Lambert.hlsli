#include "Scene.hlsli"

struct VS_OUT
{
	float4 vertex	: SV_POSITION;
	float2 texcoord	: TEXCOORD;
	float3 normal	: NORMAL;
	float3 position : POSITION;
	float3 tangent	: TANGENT;
};

struct PointLight
{
    float4 position;
    float4 color;
    float range;
    float3 padding;
};

PointLight point_light[24];
