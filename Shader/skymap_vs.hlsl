#include "skymap.hlsli"

VS_OUT main(float3 position : POSITION,
            float4 color : COLOR,
            float2 texcoord : TEXCOORD)
{
    VS_OUT vout;

    float4 clip_position = float4(position, 1.0f);

    vout.position = clip_position;
    vout.color = color;
    vout.texcoord = texcoord;

    float4 world_position = mul(clip_position, inverse_view_projection);
    vout.world_position = world_position / world_position.w;

    return vout;
}