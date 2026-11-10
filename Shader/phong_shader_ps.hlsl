#include "Lambert.hlsli"

Texture2D normal_map : register(t1);

Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);

Texture2D sky_map : register(t5);
SamplerState sky_sampler_state : register(s5);

cbuffer CbMesh : register(b1)
{
    float4 materialColor;
    float4 highlightColor;
    float highlight;
    float3 padding;
};

Texture2D DiffuseMap : register(t0);
SamplerState LinearSampler : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color =
        DiffuseMap.Sample(
            LinearSampler,
            pin.texcoord) *
        materialColor;

    // 法線
    float3 N = normalize(pin.normal);
    
    

    // 平行光源
    float3 L = normalize(-lightDirection.xyz);

    // 拡散反射
    float diffuse =
        max(0.0f, dot(N, L));

    // 視線ベクトル
    float3 V =
        normalize(cameraPosition.xyz - pin.position);

    //// ポイントライト
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;

    for (int i = 0; i < 41; ++i)
    {
        float3 LP =
            point_light[i].position.xyz -
            pin.position;

        float len = length(LP);

        if (len >= point_light[i].range)
            continue;

        LP /= len;

        float attenuation =
            saturate(
                1.0f -
                len / point_light[i].range);

        attenuation *= attenuation;

        float pd =
            max(0.0f, dot(N, LP));

        pointDiffuse +=
          point_light[i].color.rgb *pd *attenuation;
        
        float3 PR =
            reflect(-LP, N);

        float ps =
            pow(
                max(
                    0.0f,
                    dot(V, PR)),
                256.0f);

        pointSpecular +=
            point_light[i].color.rgb *
            ps *
            attenuation;
    }

    float ambient = 0.08f;

    float3 lighting =
        lightColor.rgb *
        (ambient + diffuse*0.35f);

    color.rgb *= lighting;

    // ポイントライト拡散反射
    color.rgb += pointDiffuse;

    
    // ポイントライトスペキュラ
   color.rgb += pointSpecular*0.05f;

    // スカイボックス反射
    {
        float4 skybox_color =
            SampleSkybox(
                sky_map,
                sky_sampler_state,
                reflect(-V, N));

        color.rgb =
            lerp(
                color.rgb,
                skybox_color.rgb,
                0.05f);
    }
    
   
    if (highlight>0.5f)
    {
        color.rgb *= 1.4f;

        float rim = 1.0f - saturate(dot(V, N));

        rim = pow(rim, 2.0f);

        color.rgb += rim * highlightColor.rgb*2.0f;
    }
    

        return saturate(color);
}

