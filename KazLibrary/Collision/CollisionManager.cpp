#include"../Collision/CollisionManager.h"

CollisionManager::CollisionManager()
{
}

CollisionManager::~CollisionManager()
{
}

bool CollisionManager::CheckSphereAndPlane(const Sphere &SPHERE, const Plane &PLANE, KazMath::Vec3<float> *INTER)
{
	//座標系の原点から球の中心座標への距離
	float distV = SPHERE.center->Dot(PLANE.normal);

	//平面の原点距離を減算する事で、平面と球の中心との距離が出る
	float dist = distV - PLANE.distance;

	//距離の絶対値が半径より大きければ当たってない
	if (fabs(dist) > SPHERE.radius)
	{
		return false;
	}


	//疑似光点を計算
	if (INTER)
	{
		//平面上の疑似接点を、疑似交点とする
		*INTER = PLANE.normal * -dist + *SPHERE.center;
	}
	return true;
}

bool CollisionManager::CheckSphereAndTriangle(const Sphere &SPHERE, const Triangle &TRIANGLE, KazMath::Vec3<float> *INTER)
{
	KazMath::Vec3<float> p;

	//球の中心に対する最近接点である三角形上にある点pを見つける
	ClosestPtPoint2Triangle(*SPHERE.center, TRIANGLE, &p);

	//点pと球の中心の差分ベクトル
	KazMath::Vec3<float> v = p - *SPHERE.center;

	//距離の二乗を求める
	//(同じベクトル同士の内積は三平方の定理のルート内部の式と一致する)
	float d = v.Dot(v);

	//球と三角形の距離が半径以下なら当たっていない
	if (d > SPHERE.radius * SPHERE.radius)
	{
		return false;
	}

	if (INTER)
	{
		//三角形上の最近接点
		*INTER = p;
	}

	return false;
}

bool CollisionManager::CheckRayAndPlane(const Ray &RAY, const Plane &PLANE, float *DISTANCE, KazMath::Vec3<float> *INTER)
{
	//イプシオン...誤差吸収用の0に近い微小な値
	const float epsion = 1.0 - 5.0f;
	//面法線とレイの方向ベクトルの内積
	float d1 = PLANE.normal.Dot(RAY.dir);
	//裏面に当たらない
	if (d1 > -epsion)
	{
		return false;
	}

	//始点と平面の距離(平面の法線方向)
	//面法線とレイの始点座標(位置ベクトル)の内積
	float d2 = PLANE.normal.Dot(RAY.start);

	//始点と平面の距離(平面の法線方向)
	float dist = d2 - PLANE.distance;

	//始点と平面の距離(レイ方向)
	float t = dist / -d1;

	//交点が始点より後ろにあるので、当たらない
	if (t < 0)
	{
		return false;
	}

	//距離を書き込む
	if (DISTANCE)
	{
		*DISTANCE = t;
	}

	//交点を計算
	if (INTER)
	{
		*INTER = RAY.start + RAY.dir * t;
	}

	return true;
}

bool CollisionManager::CheckRayAndTriange(const Ray &RAY, const Triangle &TRIANGLE, float *DISTANCE, KazMath::Vec3<float> *INTER)
{
	//三角形が載っている平面を算出
	Plane plane;
	KazMath::Vec3<float> interPlane;

	plane.normal = TRIANGLE.normal;
	plane.distance = TRIANGLE.normal.Dot(TRIANGLE.p0);

	//Rayと平面が当たっていなければ、当たっていない
	if (!CheckRayAndPlane(RAY, plane, DISTANCE, &interPlane))
	{
		return false;
	}

	//Rayと平面が当たっていたので、距離と交点が書き込まれた
	//Rayと平面の好転が三角形の内側にあるか判定
	const float epsilon = 1.0 - 5.0f;

	//辺p0_p1について
	KazMath::Vec3<float> pt_p0 = TRIANGLE.p0 - interPlane;
	KazMath::Vec3<float> p0_p1 = TRIANGLE.p1 - TRIANGLE.p0;
	KazMath::Vec3<float> m;
	m = pt_p0.Cross(p0_p1);

	//辺の外側であれば当たっていないので判定を打ち切る
	if (m.Dot(TRIANGLE.normal) < -epsilon)
	{
		return false;
	}


	//辺p1_p2について
	KazMath::Vec3<float> pt_p1 = TRIANGLE.p1 - interPlane;
	KazMath::Vec3<float> p1_p2 = TRIANGLE.p2 - TRIANGLE.p1;
	m = pt_p1.Cross(p1_p2);

	//辺の外側であれば当たっていないので判定を打ち切る
	if (m.Dot(TRIANGLE.normal) < -epsilon)
	{
		return false;
	}


	//辺p2_p0について
	KazMath::Vec3<float> pt_p2 = TRIANGLE.p2 - interPlane;
	KazMath::Vec3<float> p2_p0 = TRIANGLE.p0 - TRIANGLE.p2;
	m = pt_p2.Cross(p2_p0);

	//辺の外側であれば当たっていないので判定を打ち切る
	if (m.Dot(TRIANGLE.normal) < -epsilon)
	{
		return false;
	}


	if (INTER)
	{
		*INTER = interPlane;
	}

	return true;
}

bool CollisionManager::CheckRayAndSphere(const Ray &RAY, const Sphere &SPHERE, float *DISTANCE, KazMath::Vec3<float> *INTER)
{
	KazMath::Vec3<float> m = RAY.start - *SPHERE.center;
	float b = m.Dot(RAY.dir);
	float c = m.Dot(m) - SPHERE.radius * SPHERE.radius;

	//Rayの始点がSphereの外側にあり(c>0),RayがSphereから離れていく方向を指している場合(b>0),当たらない
	if (c > 0.0f && b > 0.0f)
	{
		return false;
	}

	float discr = b * b - c;
	//負の判別式はRayが球を外れていることに一致
	if (discr < 0.0f)
	{
		return false;
	}


	//Rayは球と交差している
	//交差する最小の値Tを計算
	float t = -b - sqrtf(discr);

	//tが負である場合、Rayは球の内側から開始しているのでtをゼロにクランプ
	//クランプ(値を範囲内に入れる)
	if (t < 0)
	{
		t = 0.0f;
	}

	if (DISTANCE)
	{
		*DISTANCE = t;
	}

	if (INTER)
	{
		*INTER = RAY.start + RAY.dir * t;
	}
	return true;
}

bool CollisionManager::CheckSphereAndSphere(const Sphere &SPHERE_1, const Sphere &SPHERE_2)
{
	float distance = SPHERE_1.center->Distance(*SPHERE_2.center);
	float sumRadist = SPHERE_1.radius + SPHERE_2.radius;

	return (distance <= sumRadist);
}

bool CollisionManager::CheckSquareAndSquare(const Square &SQUARE_1, const Square &SQUARE_2)
{
	std::array<KazMath::Vec3<float>, 2> size = { SQUARE_1.size / 2.0f,SQUARE_2.size / 2.0f };
	KazMath::Vec3<float>distance = SQUARE_1.center - SQUARE_2.center;
	const int square1 = 0;
	const int square2 = 1;
	bool isHitFlag = fabs(distance.x) <= size[square1].x + size[square2].x && fabs(distance.y) <= size[square1].y + size[square2].y;
	return isHitFlag;
}

bool CollisionManager::CheckThicklineAndSphere(const Sphere &SPHERE, const ModiRectangle &RECTANGLE)
{
	/*

	〇 判定の流れ

	・ 矩形を円の半径分、隣接する頂点から延びる方向に拡張する。

		 ↑			 ↑
	  ← p0___________p1 →
		 |			 |
		 |			 |
	  ← p3___________p2 →
		 ↓			 ↓

	・ 円の中心点と矩形とで外積を使った左右判定を使用して、当たり判定を行う。


	*注意 …　矩形は左上をp0にして、時計回りの順番にデータを入れてください。

	*/



	// Z軸の情報をY軸に入れる。

	Sphere sphereBuff = SPHERE;
	ModiRectangle rectangleBuff = RECTANGLE;

	sphereBuff.center->y = SPHERE.center->z;
	sphereBuff.center->z = SPHERE.center->y;

	rectangleBuff.p0.y = RECTANGLE.p0.z;
	rectangleBuff.p0.z = RECTANGLE.p0.y;

	rectangleBuff.p1.y = RECTANGLE.p1.z;
	rectangleBuff.p1.z = RECTANGLE.p1.y;

	rectangleBuff.p2.y = RECTANGLE.p2.z;
	rectangleBuff.p2.z = RECTANGLE.p2.y;

	rectangleBuff.p3.y = RECTANGLE.p3.z;
	rectangleBuff.p3.z = RECTANGLE.p3.y;


	// 矩形データを保存する。
	ModiRectangle rectangleBuffBuff = rectangleBuff;

	// 円の半径を保存する変数
	KazMath::Vec3<float> sphereRadius = { sphereBuff.radius, sphereBuff.radius, sphereBuff.radius };

	// 頂点を計算する際にデータを一時保存する変数
	KazMath::Vec3<float> dataBuff = {};
	KazMath::Vec3<float> dataBuff2 = {};


	/*-- 矩形を拡張する。 --*/

	// p0に関して拡張処理を行う。
	{

		// p1からp0までの方向ベクトルを求め、正規化させる。
		dataBuff = rectangleBuffBuff.p0 - rectangleBuffBuff.p1;
		dataBuff.Normalize();
		// p3からp0までの方向ベクトルを求め、正規化させる。
		dataBuff2 = rectangleBuffBuff.p0 - rectangleBuffBuff.p3;
		dataBuff2.Normalize();

		//// 足して正規化する
		//dataBuff += dataBuff2;
		//dataBuff = XMVector2Normalize(dataBuff);

		rectangleBuffBuff.p0 += dataBuff * sphereRadius;
		rectangleBuffBuff.p0 += dataBuff2 * sphereRadius;

	}

	// p1に関して拡張処理を行う。
	{

		// p2からp1までの方向ベクトルを求め、正規化させる。
		dataBuff = rectangleBuffBuff.p1 - rectangleBuffBuff.p2;
		dataBuff.Normalize();
		// p0からp1までの方向ベクトルを求め、正規化させる。
		dataBuff2 = rectangleBuffBuff.p1 - rectangleBuffBuff.p0;
		dataBuff2.Normalize();

		//// 足して正規化する
		//dataBuff += dataBuff2;
		//dataBuff = XMVector2Normalize(dataBuff);

		rectangleBuffBuff.p1 += dataBuff * sphereRadius;
		rectangleBuffBuff.p1 += dataBuff2 * sphereRadius;

	}

	// p2に関して拡張処理を行う。
	{

		// p3からp2までの方向ベクトルを求め、正規化させる。
		dataBuff = rectangleBuffBuff.p2 - rectangleBuffBuff.p3;
		dataBuff.Normalize();
		// p1からp2までの方向ベクトルを求め、正規化させる。
		dataBuff2 = rectangleBuffBuff.p2 - rectangleBuffBuff.p1;
		dataBuff2.Normalize();

		//// 足して正規化する
		//dataBuff += dataBuff2;
		//dataBuff = XMVector2Normalize(dataBuff);

		rectangleBuffBuff.p2 += dataBuff * sphereRadius;
		rectangleBuffBuff.p2 += dataBuff2 * sphereRadius;

	}

	// p3に関して拡張処理を行う。
	{

		// p0からp1までの方向ベクトルを求め、正規化させる。
		dataBuff = rectangleBuffBuff.p3 - rectangleBuffBuff.p0;
		dataBuff.Normalize();
		// p2からp3までの方向ベクトルを求め、正規化させる。
		dataBuff2 = rectangleBuffBuff.p3 - rectangleBuffBuff.p2;
		dataBuff2.Normalize();

		//// 足して正規化する
		//dataBuff += dataBuff2;
		//dataBuff = XMVector2Normalize(dataBuff);

		rectangleBuffBuff.p3 += dataBuff * sphereRadius;
		rectangleBuffBuff.p3 += dataBuff2 * sphereRadius;

	}

	/*-- 拡張した矩形と円の中心点を外積の左右判定を使って当たり判定を行う。 --*/

	// 判定を保存するようの変数
	std::vector<bool> isHit;

	// 外積判定をする際にベクトルを保存するための変数
	KazMath::Vec3<float> vTvVec = {};	// 頂点と頂点のベクトル
	KazMath::Vec3<float> vTPVec = {};	// 頂点と中心点のベクトル

	// p0->p1での左右判定
	{

		// 頂点と頂点ベクトルを求めて正規化
		vTvVec = rectangleBuffBuff.p1 - rectangleBuffBuff.p0;
		vTvVec.Normalize();
		// 頂点と中心点のベクトルを求めて正規化
		vTPVec = *sphereBuff.center - rectangleBuffBuff.p0;
		vTPVec.Normalize();

		// 外積を計算 +の場合右判定
		isHit.push_back(vTvVec.Cross(vTPVec).x < 0);

	}

	// p1->p2での左右判定
	{

		// 頂点と頂点ベクトルを求めて正規化
		vTvVec = rectangleBuffBuff.p2 - rectangleBuffBuff.p1;
		vTvVec.Normalize();
		// 頂点と中心点のベクトルを求めて正規化
		vTPVec = *sphereBuff.center - rectangleBuffBuff.p1;
		vTPVec.Normalize();

		// 外積を計算 +の場合右判定
		isHit.push_back(vTvVec.Cross(vTPVec).x < 0);

	}

	// p2->p3での左右判定
	{

		// 頂点と頂点ベクトルを求めて正規化
		vTvVec = rectangleBuffBuff.p3 - rectangleBuffBuff.p2;
		vTvVec.Normalize();
		// 頂点と中心点のベクトルを求めて正規化
		vTPVec = *sphereBuff.center - rectangleBuffBuff.p2;
		vTPVec.Normalize();

		// 外積を計算 +の場合右判定
		isHit.push_back(vTvVec.Cross(vTPVec).x < 0);

	}

	// p3->p0での左右判定
	{

		// 頂点と頂点ベクトルを求めて正規化
		vTvVec = rectangleBuffBuff.p0 - rectangleBuffBuff.p3;
		vTvVec.Normalize();
		// 頂点と中心点のベクトルを求めて正規化
		vTPVec = *sphereBuff.center - rectangleBuffBuff.p3;
		vTPVec.Normalize();

		// 外積を計算 +の場合右判定
		isHit.push_back(vTvVec.Cross(vTPVec).x < 0);

	}

	// 結果を計算
	bool isHitResult = true;
	for (int i = 0; i < 4; ++i)
	{
		isHitResult = isHit[i] && isHitResult;
	}


	return isHitResult;
}

std::array<KazMath::Vec2<float>, 2> CollisionManager::CheckCircleAndRay(const Sphere &SPHERE, const KazMath::Vec2<float> &START_POS, const KazMath::Vec2<float> &END_POS)
{
	//ax + by + c = 0 の定数項
	float a = END_POS.y - START_POS.y;
	float b = START_POS.x - END_POS.x;
	float c = -a * START_POS.x - b * START_POS.y;

	//円の中心から直線までの距離
	//mag(a, b) = √a^2+b^2
	float d = abs((a * SPHERE.center->x + b * SPHERE.center->y + c) / (sqrtf(a * a) + b * b));

	//直線の垂線とX軸と平行な線がなす角度θ
	float theta = atan2(b, a);

	if (d > SPHERE.radius)
	{
		std::array<KazMath::Vec2<float>, 2>resultArray;
		return resultArray;
	}
	else if (d == SPHERE.radius)
	{
		//場合わけ
		if (a * SPHERE.center->x + b * SPHERE.center->y + c > 0)
		{
			theta += KazMath::PI_2F;
		}

		float crossX = SPHERE.radius * cos(theta) + SPHERE.center->x;
		float crossY = SPHERE.radius * sin(theta) + SPHERE.center->y;

		KazMath::Vec2<float> result(crossX, crossY);
		std::array<KazMath::Vec2<float>, 2>resultArray;
		resultArray[0] = result;
		return resultArray;
	}
	else
	{
		KazMath::Vec2<float> result1;
		KazMath::Vec2<float> result2;

		//alphaとbetaの角度を求める
		float alpha, beta, phi;
		phi = acos(d / SPHERE.radius);
		alpha = theta - phi;
		beta = theta + phi;

		//場合わけ
		if (a * SPHERE.center->x + b * SPHERE.center->y + c > 0) {
			alpha += KazMath::PI_2F;
			beta += KazMath::PI_2F;
		}

		//交点の座標を求める
		result1.x = SPHERE.radius * cos(alpha) + SPHERE.center->x;
		result1.y = SPHERE.radius * sin(alpha) + SPHERE.center->y;

		result2.x = SPHERE.radius * cos(beta) + SPHERE.center->x;
		result2.y = SPHERE.radius * sin(beta) + SPHERE.center->y;


		std::array<KazMath::Vec2<float>, 2>resultArray;
		resultArray[0] = result1;
		resultArray[1] = result2;

		return resultArray;
	}
}

bool CollisionManager::CheckRayAndPlane3D(const Ray &RAY, const ModiRectangle &MODI, float *DISTANCE)
{
	//三角形が載っている平面を算出
	std::array<Triangle, 2> lTriangle;

	lTriangle[0].p0 = MODI.p0;
	lTriangle[0].p1 = MODI.p1;
	lTriangle[0].p2 = MODI.p2;
	lTriangle[0].ComputeNormal();

	lTriangle[1].p0 = MODI.p2;
	lTriangle[1].p1 = MODI.p3;
	lTriangle[1].p2 = MODI.p1;
	lTriangle[1].ComputeNormal();

	//Rayと平面が当たっていなければ、当たっていない
	if (CheckRayAndTriange(RAY, lTriangle[0], DISTANCE, nullptr) || CheckRayAndTriange(RAY, lTriangle[1], DISTANCE, nullptr))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CollisionManager::IsIntersected(const KazMath::Vec3<float> &START_POS_A, const KazMath::Vec3<float> &END_POS_A, const KazMath::Vec3<float> &START_POS_B, const KazMath::Vec3<float> &END_POS_B)
{
	//--線分の外積を計算して交差判定を行う--
	//第一回 線分Aから見たBの交差判定
	KazMath::Vec3<float> lBuffA = END_POS_A - START_POS_A;
	lBuffA.Normalize();
	KazMath::Vec3<float> lBuffB = START_POS_B - START_POS_A;
	lBuffB.Normalize();
	KazMath::Vec3<float> lBuffC = lBuffA;
	lBuffC.Normalize();
	KazMath::Vec3<float> lBuffD = END_POS_B - START_POS_A;
	lBuffD.Normalize();

	KazMath::Vec3<float> lBuffE = lBuffA.Cross(lBuffB);
	KazMath::Vec3<float> lBuffF = lBuffC.Cross(lBuffD);
	KazMath::Vec3<float> lResult1 = lBuffE * lBuffF;

	//第二回 線分Bから見たAの交差判定
	lBuffA = END_POS_B - END_POS_A;
	lBuffA.Normalize();
	lBuffB = START_POS_A - START_POS_B;
	lBuffB.Normalize();
	lBuffC = lBuffA;
	lBuffC.Normalize();
	lBuffD = END_POS_A - START_POS_A;
	lBuffD.Normalize();

	lBuffE = lBuffA.Cross(lBuffB);
	lBuffF = lBuffC.Cross(lBuffD);
	KazMath::Vec3<float> lResult2 = lBuffE * lBuffF;

	//線分が交差している時は、線分から見て交差判定したい線分の端点2つが両側にある時。
	//外積で左右判定をすると、交差している時は値の結果が+と-になる。
	//つまり両方の外積を掛けて結果が-になった場合のみ交差している。
	//線分AからみてBを判定、線分BからみてAを判定と二通り判定を行う。
	//この2つの判定結果を掛けた時に-、若しくは0の時2つの線分は交差している。
	if (lResult1.x < 0 && lResult2.x < 0)
	{
		return true;
	}
	return false;
}

KazMath::Vec3<float> CollisionManager::CalIntersectPoint(const KazMath::Vec3<float> &START_POS_A, const KazMath::Vec3<float> &END_POS_A, const KazMath::Vec3<float> &START_POS_B, const KazMath::Vec3<float> &END_POS_B)
{
	//交点を求める この式は資料そのまま
	KazMath::Vec3<float> lBuff = END_POS_B - START_POS_B;
	double lD1 = fabs(lBuff.Cross(START_POS_A - START_POS_B).x);
	double lD2 = fabs(lBuff.Cross(END_POS_A - START_POS_B).x);
	double lT = lD1 / (lD1 + lD2);
	return START_POS_A + (END_POS_A - START_POS_A) * static_cast<float>(lT);
}

void CollisionManager::ClosestPtPoint2Triangle(const KazMath::Vec3<float> &point, const Triangle &triangle, KazMath::Vec3<float> *closest)
{
	// pointがp0の外側の頂点領域の中にあるかどうかチェック
	KazMath::Vec3<float> p0_p1 = triangle.p1 - triangle.p0;
	KazMath::Vec3<float> p0_p2 = triangle.p2 - triangle.p0;
	KazMath::Vec3<float> p0_pt = point - triangle.p0;

	float d1 = p0_p1.Dot(p0_pt);
	float d2 = p0_p2.Dot(p0_pt);

	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		// p0が最近傍
		*closest = triangle.p0;
		return;
	}

	// pointがp1の外側の頂点領域の中にあるかどうかチェック
	KazMath::Vec3<float> p1_pt = point - triangle.p1;

	float d3 = p0_p1.Dot(p1_pt);
	float d4 = p0_p2.Dot(p1_pt);

	if (d3 >= 0.0f && d4 <= d3)
	{
		// p1が最近傍
		*closest = triangle.p1;
		return;
	}

	// pointがp0_p1の辺領域の中にあるかどうかチェックし、あればpointのp0_p1上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
	{
		float v = d1 / (d1 - d3);
		*closest = triangle.p0 + p0_p1 * v;
		return;
	}

	// pointがp2の外側の頂点領域の中にあるかどうかチェック
	KazMath::Vec3<float> p2_pt = point - triangle.p2;

	float d5 = p0_p1.Dot(p2_pt);
	float d6 = p0_p2.Dot(p2_pt);
	if (d6 >= 0.0f && d5 <= d6)
	{
		*closest = triangle.p2;
		return;
	}

	// pointがp0_p2の辺領域の中にあるかどうかチェックし、あればpointのp0_p2上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
	{
		float w = d2 / (d2 - d6);
		*closest = triangle.p0 + p0_p2 * w;
		return;
	}

	// pointがp1_p2の辺領域の中にあるかどうかチェックし、あればpointのp1_p2上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f)
	{
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		*closest = triangle.p1 + (triangle.p2 - triangle.p1) * w;
		return;
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	*closest = triangle.p0 + p0_p1 * v + p0_p2 * w;
}
