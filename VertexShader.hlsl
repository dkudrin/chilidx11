struct VertexShaderOutput
{
    float3 color : Color;
    float4 pos : SV_Position;
};

cbuffer ConstantBuffer
{
    matrix transformMatrix; // keyword "matrix" indicates 4x4 float matrix
};

VertexShaderOutput main(float3 pos : Position, float3 color : Color)
{
    VertexShaderOutput vso;
    vso.pos = mul(float4(pos, 1.0f), transformMatrix); // Умножение вактора на матрицу - вектор слева, матрица справа. mul - втсроенная функция
    vso.color = color;
    return vso;
}