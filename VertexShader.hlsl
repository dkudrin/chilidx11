struct VertexShaderOutput
{
    float3 color : Color;
    float4 pos : SV_Position;
};

cbuffer ConstantBuffer
{
    row_major matrix transformMatrix; // keyword "matrix" indicates 4x4 float matrix
    // row_major нужен для того чтобы на CPU и GPU матрицы обрабатывались одинаково (ибо изначально они обрабатываются по-разному - GPU воспринимает матрицу в транспонированном виде)
    // позже мы сделаем транспонирование на GPU
};

VertexShaderOutput main(float2 pos : Position, float3 color : Color)
{
    VertexShaderOutput vso;
    vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transformMatrix); // Умножение вактора на матрицу - вектор слева, матрица справа. mul - втсроенная функция
    vso.color = color;
    return vso;
}