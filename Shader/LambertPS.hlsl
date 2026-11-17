#include "Lambert.hlsli"

cbuffer CbMesh : register(b1)
{
    float4 materialColor;
    float4 highlightColor;
    float highlight;
    float3 padding;
};

Texture2D DiffuseMap		: register(t0);
SamplerState LinearSampler	: register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = DiffuseMap.Sample(LinearSampler, pin.texcoord) * materialColor;

	float3 N = normalize(pin.normal);
	float3 L = normalize(-lightDirection.xyz);
	float power = max(0, dot(L, N));

	power = power * 0.5 + 0.5f;

	color.rgb *= lightColor.rgb * power;
	
	
    if (highlight > 0.5f)
    {
        color.rgb = highlightColor.rgb;
    }

	return color;
}
