#include "phong_shader.hlsli"

Texture2D color_map : register(t0);
SamplerState color_sampler_state : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 diffuse_color = color_map.Sample(color_sampler_state, pin.texcoord);
	
    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    float3 L = normalize(directional_light_direction.xyz);
   // float3 N = normalize(pin.world_normal.xyz);
	
    float3 ambient = ambient_color.rgb * ka.rgb;
	
	
	
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}