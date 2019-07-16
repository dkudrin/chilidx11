cbuffer ConstantBuffer
{
    matrix transformMatrix; // keyword "matrix" indicates 4x4 float matrix
};

float4 main(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), transformMatrix); // Умножение вактора на матрицу - вектор слева, матрица справа. mul - втсроенная функция
}