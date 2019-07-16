struct VertexShaderOutput
{
    float3 color : Color;
    float4 pos : SV_Position;
};

cbuffer ConstantBuffer
{
    matrix transformMatrix; // keyword "matrix" indicates 4x4 float matrix
};

VertexShaderOutput main(float2 pos : Position, float3 color : Color)
{
    VertexShaderOutput vso;
    vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transformMatrix); // ��������� ������� �� ������� - ������ �����, ������� ������. mul - ���������� �������
    vso.color = color;
    return vso;
}