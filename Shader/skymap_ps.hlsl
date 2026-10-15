#include "skymap.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float3 E = normalize(
        pin.world_position.xyz - camera_position.xyz
    );

    //return texture0.Sample(sampler0, pin.texcoord);
    
    return SampleSkybox(texture0, sampler0, E);
}