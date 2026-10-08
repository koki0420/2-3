//#include "phong_shader.hlsli"
#include "Lambert.hlsli"

//Texture2D color_map : register(t0);
//SamplerState color_sampler_state : register(s0);
Texture2D normal_map : register(t1);

Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);


cbuffer CbMesh : register(b1)
{
    float4 materialColor;
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

  
    
    //// �@��
    float3 N = normalize(pin.normal);
    
    

    
    
    //// ���C�g����
    float3 L = normalize(-lightDirection.xyz);

    //// �g�U����
    float diffuse =
        max(0.0f, dot(N, L));

    // �J��������
    float3 V =
        normalize(cameraPosition.xyz - pin.position);

    //// ���˃x�N�g��
    //float3 R =
    //    reflect(-L, N);

    //// ���ʔ���
    //float specular =
    //    pow(
    //        max(0.0f, dot(V, R)),
    //        32.0f);

    
// �|�C���g���C�g
    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;


  
    
    
    //for (int i = 0; i < 19; i++)
    //{
        

       
    //    float3 LP =
    //        point_light[i].position.xyz -
    //        pin.position;
        
          
    //   float len = length(LP);
        
     
      
        
       
    //    if (len >= point_light[i].range)
    //        continue;

    //    LP /= len;

    //    float attenuation =
    //        saturate(
    //            1.0f -
    //            len / point_light[i].range);
      

    //    attenuation *= attenuation;
        

    //    float pd =
    //        max(0.0f, dot(N, LP));

       
        

        
    //    pointDiffuse +=
    //     point_light[i].color.rgb *
    //     (0.3f + pd) *
    //     attenuation;


    //    float3 PR =
    //        reflect(-LP, N);

    //    float ps =
    //        pow(
    //            max(
    //                0.0f,
    //                dot(V, PR)),
    //            32.0f);

    //    pointSpecular +=
    //        point_light[i].color.rgb *
    //        ps *
    //        attenuation;
        
        
      
       

       
    
    //}

    
    
    
    

    
  
 
    float ambient = 0.1f;

    float3 lighting =
        lightColor.rgb *
        (ambient + diffuse);

    //lighting += pointDiffuse;

   
     //color.rgb *= pointDiffuse;
    color.rgb *= lighting;
    
    //color.rgb *= (ambient + pointDiffuse);
   // color.rgb += pointSpecular;

     
    

    
    
    return color;

}

//float4 main(VS_OUT pin) : SV_TARGET
//{
//    float4 diffuse_color = color_map.Sample(color_sampler_state, pin.texcoord);
	
//   // float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
//    float3 E = normalize(pin.world_position.xyz - camera_position.xyz);
    
//    float3 L = normalize(directional_light_direction.xyz);
//   // float3 N = normalize(pin.world_normal.xyz);
//    float3x3 mat = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal) };
//   // float3 N = normal_map.Sample(color_sampler_state, pin.texcoord).rgb;
    
//    float3 N = normalize(pin.normal);

//    //�m�[�}���e�N�X�`���@�������[���h�֕ϊ�
//    N = normalize(mul(N * 2.0f - 1.0f, mat));
    
//    float3 ambient = ambient_color.rgb * ka.rgb;
//    ambient += CalHemiSphereLight(N, float3(0, 1, 0), sky_color.rgb, ground_color.rgb, hemisphere_weight);
//    //float3 directional_diffuse = 0;
//    //{
//    //    float3 power = saturate(dot(N, -L));
//    //    directional_diffuse = directional_light_color.rgb * power * kd.rgb;

//    //}
    
//    float3 directional_diffuse = CalcLambert(N, L, directional_light_color.rgb, kd.rgb);
    
//    //float3 directional_specular = 0;
//    //{
//    //    float3 R = reflect(L, N);
//    //    float power = max(dot(-E, R), 0);
//    //    power = pow(power, 128);
//    //    directional_specular = directional_light_color.rgb * power * ks.rgb;
//    //}
//    float3 directional_specular = CalcPhongSpecular(N, L, E, directional_light_color.rgb, ks.rgb);
    
    
//    {
//        //�V���h�E�}�b�v����[�x�l�擾
//        float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
//        //�[�x�l���r���ĉe���ǂ����𔻒肷��
//        if (pin.shadow_texcoord.z - depth > shadow_bias)
//        {
//            directional_diffuse *= shadow_color.rgb;
//            directional_specular *= shadow_color.rgb;
//        }

//    }
    
    
    
//    //�_�����̏���
//    float3 point_diffuse = 0;
//    float3 point_specular = 0;
//    for (int i = 0; i < 8; ++i)
//    {
//        float3 LP = pin.world_position.xyz - point_light[i].position.xyz;
//        float len = length(LP);
//        if (len >= point_light[i].range)
//            continue;
//        float attenuateLength = saturate(1.0f - len / point_light[i].range);
//        float attenuation = attenuateLength * attenuateLength;
//        LP /= len;
//        point_diffuse += CalcLambert(N, LP, point_light[i].color.rgb, kd.rgb) * attenuation;
//        point_specular += CalcPhongSpecular(N, LP, E, point_light[i].color.rgb, ks.rgb) * attenuation;

//    }
    
//    //�X�|�b�g���C�g�̏���
//    float3 spot_diffuse = 0;
//    float3 spot_specular = 0;
//    for (int j = 0; j < 8; j++)
//    {
//        float3 LP = pin.world_position.xyz - spot_light[j].position.xyz;
//        float len = length(LP);
//        if (len >= spot_light[j].range)
//            continue;
//        float attenuateLength = saturate(1.0f - len / spot_light[j].range);
//        float attenuation = attenuateLength * attenuateLength;
//        LP /= len;
//        float3 spotDirection = normalize(spot_light[j].direction.xyz);
//        float angle = dot(spotDirection, LP);
//        float area = spot_light[j].innerCorn - spot_light[j].outerCorn;
//        attenuation *= saturate(1.0f - (spot_light[j].innerCorn - angle) / area);
//        spot_diffuse += CalcLambert(N, LP, spot_light[j].color.rgb, kd.rgb) * attenuation;
//        spot_specular += CalcPhongSpecular(N, LP, E, spot_light[j].color.rgb, ks.rgb) * attenuation;
        
//    }
    
    
    
//    float3 rim_color = CalcRimLight(N, E, L, directional_light_color.rgb);
//    float4 color = float4(diffuse_color.rgb * (ambient + directional_diffuse + point_diffuse + spot_diffuse), diffuse_color.a);
//    color.rgb += directional_specular + spot_specular + point_specular;
//   // color.rgb += rim_color;
  
//   // color = CalcFog(color, fog_color, fog_range.xy, length(pin.world_position.xyz - camera_position.xyz));
  
    
    
//    return color;
//}



