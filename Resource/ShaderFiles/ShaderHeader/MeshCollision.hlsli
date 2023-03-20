
//BBを形づくる為のデータ
struct BoundingBoxData
{
    float3 minPos;
    float3 maxPos;
};

//パーティクルの当たり判定情報
struct ParticleHitBoxData
{
    float3 pos;
    float4 color;
    uint meshID;
    uint3 id;
};

struct MeshHitBox
{
    float3 pos;
    uint meshID;
    uint3 id;
};

struct MeshSphereHitData
{
    float3 meshPos;
    uint meshID;
    uint3 id;
    //対象球の座標
    float3 circlePos;
};

struct HitIDData
{
    uint meshID;
    uint3 id;
};