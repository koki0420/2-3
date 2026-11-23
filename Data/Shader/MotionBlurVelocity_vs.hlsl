#include "Skinning.hlsli"

// 前フレームのボーン行列
cbuffer CbPreviousSkeleton : register(b4)
{
    row_major float4x4 previousBoneTransforms[256];
};

// 現在・前フレームのViewProjection
cbuffer CbMotionBlur : register(b0)
{
    row_major float4x4 currentViewProjection;
    row_major float4x4 previousViewProjection;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 currentClip : TEXCOORD0;
    float4 previousClip : TEXCOORD1;
};

// 前フレームのスキニング
float4 PreviousSkinningPosition(
    float4 position,
    float4 boneWeights,
    uint4 boneIndices)
{
    float4 p = float4(0, 0, 0, 0);

    [unroll]
    for (int i = 0; i < 4; i++)
    {
        p += boneWeights[i] *
            mul(position, previousBoneTransforms[boneIndices[i]]);
    }

    return p;
}

VS_OUT main(
    float3 position : POSITION,
    float3 normal : NORMAL,
    float3 tangent : TANGENT,
    float2 texcoord : TEXCOORD,
    float4 color : COLOR,
    float4 boneWeights : BONE_WEIGHTS,
    uint4 boneIndices : BONE_INDICES)
{
    VS_OUT output = (VS_OUT) 0;

    float4 localPosition = float4(position, 1.0f);

    // 現在のスキニング
    float4 currentPosition = SkinningPosition(
        localPosition,
        boneWeights,
        boneIndices);

    // 前フレームのスキニング
    float4 previousPosition = PreviousSkinningPosition(
        localPosition,
        boneWeights,
        boneIndices);

    // 現在のクリップ座標
    output.currentClip = mul(
        currentPosition,
        currentViewProjection);

    // 前フレームのクリップ座標
    output.previousClip = mul(
        previousPosition,
        previousViewProjection);

    // ラスタライズには現在の座標を使用
    output.position = output.currentClip;

    return output;
}