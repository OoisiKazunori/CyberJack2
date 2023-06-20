
//円周率
static const float PI = 3.141592653589f;

//カメラ用の定数バッファ
struct CameraConstBufferData
{
    matrix mtxView; //ビュー行列
    matrix mtxProj; //プロジェクション行列
    matrix mtxViewInv; //逆ビュー行列
    matrix mtxProjInv; //逆プロジェクション行列
};

//定数バッファ
struct ConstBufferData
{
    CameraConstBufferData camera;
};

//頂点情報
struct Vertex
{
    float3 pos;
    float3 normal;
    float2 uv;
    float3 tangent;
    float3 binormal;
};

//ペイロード
struct Payload
{
    float3 color_; //色情報
};

struct MyAttribute
{
    float2 barys;
};

//barysを計算
inline float3 CalcBarycentrics(float2 Barys)
{
    return float3(1.0 - Barys.x - Barys.y, Barys.x, Barys.y);
}

//当たった位置の情報を取得する関数
Vertex GetHitVertex(MyAttribute attrib, StructuredBuffer<Vertex> vertexBuffer, StructuredBuffer<uint> indexBuffer, inout Vertex meshInfo[3])
{
    Vertex v = (Vertex) 0;
    float3 barycentrics = CalcBarycentrics(attrib.barys);
    uint vertexId = PrimitiveIndex() * 3; //Triangle List のため.

    float weights[3] =
    {
        barycentrics.x, barycentrics.y, barycentrics.z
    };

    for (int index = 0; index < 3; ++index)
    {
        uint vtxIndex = indexBuffer[vertexId + index];
        float w = weights[index];
        v.pos += vertexBuffer[vtxIndex].pos * w;
        v.normal += vertexBuffer[vtxIndex].normal * w;
        v.uv += vertexBuffer[vtxIndex].uv * w;
        
        //メッシュの情報を保存。
        meshInfo[index].pos = mul(float4(vertexBuffer[vtxIndex].pos, 1), ObjectToWorld4x3());
        meshInfo[index].normal = normalize(mul(vertexBuffer[vtxIndex].normal, (float3x3) ObjectToWorld4x3()));
        meshInfo[index].uv = vertexBuffer[vtxIndex].uv;
    }

    return v;
}

//指定の頂点の衝突したメッシュ上での重心座標を求める。
float3 CalcVertexBarys(float3 HitVertex, float3 VertexA, float3 VertexB, float3 VertexC)
{
    
    float3 e0 = VertexB - VertexA;
    float3 e1 = VertexC - VertexA;
    float3 e2 = HitVertex - VertexA;
    float d00 = dot(e0, e0);
    float d01 = dot(e0, e1);
    float d11 = dot(e1, e1);
    float d20 = dot(e2, e0);
    float d21 = dot(e2, e1);
    float denom = 1.0 / (d00 * d11 - d01 * d01);
    float v = (d11 * d20 - d01 * d21) * denom;
    float w = (d00 * d21 - d01 * d20) * denom;
    float u = 1.0 - v - w;
    return float3(u, v, w);
    
}