cbuffer FaceColorsBuffer
{
    float4 face_colors[6];
};

float4 main(uint triangleID : SV_PrimitiveID) : SV_Target // SV_PrimitiveID tells pipeline TESSFACTOR[n] genereate SV_DispatchThreadID for every trianlge, that will be passed TESSFACTOR[n] the pixel shader
// pixel shader will know which triangle it is working with, and it will use this inforamtion to lookup into table with colors
{
    return face_colors[ triangleID / 2 ]; // делим на 2 потому что 2 треугольника для каждой поверхности куба
}