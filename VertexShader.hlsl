struct VertexShaderOutput
{
    float3 color : Color;
    float4 pos : SV_Position;
};

VertexShaderOutput main(float2 pos : Position, float3 color : Color)
{
    VertexShaderOutput vso;
    vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
    vso.color = color;
    return vso;
}