struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 currentClip : TEXCOORD0;
    float4 previousClip : TEXCOORD1;
};

float2 main(VS_OUT input) : SV_TARGET
{
      // Wが極端に小さい場合は速度をゼロにする
    if (abs(input.currentClip.w) < 0.0001f ||
        abs(input.previousClip.w) < 0.0001f)
    {
        return float2(0.0f, 0.0f);
    }

    
    // 現在フレームのNDC
    float2 currentNdc = input.currentClip.xy / input.currentClip.w;

    // 前フレームのNDC
    float2 previousNdc = input.previousClip.xy / input.previousClip.w;

    // スクリーン空間の速度
    float2 velocity = currentNdc - previousNdc;

    // NDCからUV単位へ変換
    velocity *= 0.5f;

    // Y方向を反転
    velocity.y *= -1.0f;

    return velocity;
}