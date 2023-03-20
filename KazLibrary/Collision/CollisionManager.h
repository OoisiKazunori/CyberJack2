#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../Render/KazRender.h"
#include"../KazLibrary/Math/KazMath.h"

struct Triangle
{
	KazMath::Vec3<float> p0;
	KazMath::Vec3<float> p1;
	KazMath::Vec3<float> p2;
	KazMath::Vec3<float> normal;

	void ComputeNormal()
	{
		KazMath::Vec3<float> p0_p1 = p1 - p0;
		KazMath::Vec3<float> p0_p2 = p2 - p0;

		//外積により、2辺に垂直なベクトルを算出する
		normal = p0_p1.Cross(p0_p2);
		normal.Normalize();
	}
};

struct Ray
{
	//始点座標
	KazMath::Vec3<float> start;
	//方向
	KazMath::Vec3<float> dir;

	Ray() :start(), dir({ 1.0f,0.0f,0.0f })
	{};
};

struct Sphere
{
	// 中心座標
	KazMath::Vec3<float>* center;
	// 半径
	float radius;

	Sphere() : radius(1.0f), center(nullptr)
	{};
	Sphere(KazMath::Vec3<float> *CENTER_POS, float RADIUS) :center(CENTER_POS), radius(RADIUS)
	{};

	void operator=(const Sphere &rhs)
	{
		center = rhs.center;
		radius = rhs.radius;
	};
};

struct Plane
{
	// 法線ベクトル
	KazMath::Vec3<float> normal;
	// 原点(0,0,0)からの距離
	float distance;

	Plane() :normal({ 0.0f,1.0f,0.0f }), distance(0.0f)
	{};
};

struct Square
{
	KazMath::Vec3<float> center;
	KazMath::Vec3<float> size;
};

struct ModiRectangle
{
	/*

		データは下の絵のように左上(p0)から時計回りに入れてください。

	   p0___________p1
		|			|
		|			|
	   p3___________p2

	*/

	KazMath::Vec3<float> p0;
	KazMath::Vec3<float> p1;
	KazMath::Vec3<float> p2;
	KazMath::Vec3<float> p3;
	KazMath::Vec3<float> normal;

	void SetNormal()
	{
		KazMath::Vec3<float> p0_p1 = p1 - p0;
		KazMath::Vec3<float> p0_p2 = p2 - p0;
		KazMath::Vec3<float> p0_p3 = p3 - p0;

		//外積により、2辺に垂直なベクトルを算出する
		normal = p0_p1.Cross(p0_p2);
		normal.Normalize();
	}
};

class CollisionManager :public ISingleton<CollisionManager> {
public:
	CollisionManager();
	~CollisionManager();

	/// <summary>
	/// 板と球の判定を取ります
	/// </summary>
	/// <param name="SPHERE">球データ</param>
	/// <param name="PLANE">板データ</param>
	/// <param name="INTER">交点</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckSphereAndPlane(const Sphere& SPHERE, const Plane& PLANE, KazMath::Vec3<float>* INTER = nullptr);

	/// <summary>
	/// 球と三角形の判定を取ります
	/// </summary>
	/// <param name="SPHERE">球データ</param>
	/// <param name="TRIANGLE">三角形データ</param>
	/// <param name="INTER">交点</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckSphereAndTriangle(const Sphere& SPHERE, const Triangle& TRIANGLE, KazMath::Vec3<float> *INTER = nullptr);

	/// <summary>
	/// 線と板の判定を取ります
	/// </summary>
	/// <param name="RAY">線データ</param>
	/// <param name="PLANE">板データ</param>
	/// <param name="DISTANCE">距離</param>
	/// <param name="INTER">交点</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckRayAndPlane(const Ray& RAY, const Plane& PLANE, float* DISTANCE, KazMath::Vec3<float> * INTER);

	/// <summary>
	/// 線と三角形の判定を取ります
	/// </summary>
	/// <param name="RAY">線データ</param>
	/// <param name="TRIANGLE">三角形データ</param>
	/// <param name="DISTANCE">距離</param>
	/// <param name="INTER">交点</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckRayAndTriange(const Ray& RAY, const Triangle& TRIANGLE, float* DISTANCE = nullptr, KazMath::Vec3<float> * INTER = nullptr);

	/// <summary>
	/// 線と球の判定を取ります
	/// </summary>
	/// <param name="RAY">線データ</param>
	/// <param name="SPHERE">球データ</param>
	/// <param name="DISTANCE">距離</param>
	/// <param name="INTER">交点</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckRayAndSphere(const Ray& RAY, const Sphere& SPHERE, float* DISTANCE = nullptr, KazMath::Vec3<float> * INTER = nullptr);


	/// <summary>
	/// 球と球の判定を取ります
	/// </summary>
	/// <param name="SPHERE_1">球データ1</param>
	/// <param name="SPHERE_2">球データ2</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckSphereAndSphere(const Sphere& SPHERE_1, const Sphere& SPHERE_2);

	/// <summary>
	/// 長方形と長方形の判定を取ります
	/// </summary>
	/// <param name="SQUARE_1">四角データ1</param>
	/// <param name="SQUARE_2">四角データ2</param>
	/// <returns>true...衝突する,false...衝突していない</returns>
	bool CheckSquareAndSquare(const Square& SQUARE_1, const Square& SQUARE_2);

	/// <summary>
	/// 回転した矩形と球の判定を取ります
	/// </summary>
	/// <param name="SPHERE"></param>
	/// <param name="RAY"></param>
	/// <returns></returns>
	bool CheckThicklineAndSphere(const Sphere& SPHERE, const ModiRectangle& RECTANGLE);


	std::array<KazMath::Vec2<float>, 2> CheckCircleAndRay(const Sphere &SPHERE, const KazMath::Vec2<float> &START_POS, const KazMath::Vec2<float> &END_POS);

	/// <summary>
	/// 板ポリと線分の判定
	/// </summary>
	bool CheckRayAndPlane3D(const Ray &RAY, const ModiRectangle &MODI, float *DISTANCE = nullptr);


	//線分と線分の当たり判定
	bool IsIntersected(const KazMath::Vec3<float> &START_POS_A, const KazMath::Vec3<float> &END_POS_A, const KazMath::Vec3<float> &START_POS_B, const KazMath::Vec3<float> &END_POS_B);

	// 線分と線分の交点を求める処理
	KazMath::Vec3<float> CalIntersectPoint(const KazMath::Vec3<float> &START_POS_A, const KazMath::Vec3<float> &END_POS_A, const KazMath::Vec3<float> &START_POS_B, const KazMath::Vec3<float> &END_POS_B);

	friend ISingleton<CollisionManager>;
private:

	//点と三角形の最近接点
	void ClosestPtPoint2Triangle(const KazMath::Vec3<float> &point, const Triangle &triangle, KazMath::Vec3<float> *closest);


	float CaluCross(DirectX::XMFLOAT2 a, DirectX::XMFLOAT2 b, DirectX::XMFLOAT2 c)
	{
		return (b.x - a.x) * ((c.y * -1) - (a.y * -1)) - (c.x - a.x) * ((b.y * -1) - (a.y * -1));
	}
};

