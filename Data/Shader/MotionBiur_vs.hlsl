struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

VS_OUT main(uint vertexID : SV_VertexID)
{
    VS_OUT output;
    
    float2 uv = float2((vertexID << 1) & 2, vertexID & 2);
    
    output.texcoord = uv;
    
    output.position = float4(
        uv.x * 2.0f - 1.0f,
        1.0f -uv.y * 2.0f,
        0.0f,1.0f);
    
    return output;

}