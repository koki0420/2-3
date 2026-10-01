

#include "Lambert.hlsli"
#include "Skinning.hlsli"



VS_OUT main(
    float3 position : POSITION,
    float3 normal : NORMAL,
    float3 tangent : TANGENT,
    float2 texcoord : TEXCOORD,
    float4 color : COLOR,
    float4 boneWeights : BONE_WEIGHTS,
    uint4 boneIndices : BONE_INDICES)

{
    
    
    VS_OUT vout = (VS_OUT) 0;

    float4 pos = float4(position, 1.0f);

    pos = SkinningPosition(
        pos,
        boneWeights,
        boneIndices);
    
    

    vout.vertex = mul(pos, viewProjection);
    
   

    vout.position = pos.xyz;

    vout.texcoord = texcoord;

    vout.normal = normalize(
        SkinningVector(
            normal,
            boneWeights,
            boneIndices));

    
  

    
    return vout;
   


}

