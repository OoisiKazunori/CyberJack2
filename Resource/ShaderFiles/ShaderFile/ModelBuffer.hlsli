//アルベド用のバッファ
Texture2D<float4>AlbedoTex:register(t0);
//法線マップ用のバッファ
Texture2D<float4>NormalTex:register(t1);
//ラフネス、メタルネス用のバッファ
Texture2D<float4>MetalnessRoughnessTex:register(t2);

SamplerState smp :register(s0);

cbuffer MaterialID : register(b1)
{
    //0...何もしない、1...反射する、2...屈折する
    uint raytracingId;
}

//変換行列の計算の参考サイト
//https://coposuke.hateblo.jp/entry/2020/12/21/144327

//ローカル空間 → 接空間 の変換行列
float3 CalucurateLocalToTangent(float3 localVector,float3 normal,float3 tangent,float3 binNoraml)
{
    float3 tangentVector = float3(dot(tangent,localVector),dot(binNoraml,localVector),dot(normal,localVector));
    return tangentVector;
}

//接空間 → ローカル空間の変換行列
float3 CalucurateTangentToLocal(float3 tangentVector,float3 normal,float3 tangent,float3 binNoraml)
{
    float3 localVector = tangent * tangentVector.x + binNoraml * tangentVector.y + normal * tangentVector.z;
    return localVector;
}

//接空間の逆行列
matrix InvTangentMatrix(float3 tangent,float3 binormal,float3 normal)
{
   float4x4 mat =
   {
        float4(tangent, 0.0f),
        float4(binormal,0.0f),
        float4(normal  ,0.0f),
        float4(0,0,0,1)
    };
    //正規直交系なので転置すれば逆行列になる
   return transpose(mat);
}