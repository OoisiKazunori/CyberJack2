
struct CircleData
{
    float3 pos;
    float radius;
};

struct MeshHitBoxData
{
    float3 pos;
    float radius;
    uint id;
};

bool CheckCircleAndCircle(CircleData A,CircleData B)
{
    return distance(A.pos,B.pos) <= A.radius + B.radius;
};