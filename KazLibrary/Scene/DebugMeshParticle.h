#pragma once
#include"../Scene/SceneBase.h"
#include"../Render/BackGroundForDebug.h"
#include"../Render/KazRender.h"
#include"../Game/Helper/CameraWork.h"
#include"../Game/GameRenderTarget.h"
#include"../Game/Effect/MeshParticleEmitter.h"
#include"../Game/Effect/DeadParticle.h"
#include"../KazLibrary/Math/KazMath.h"
#include"../Game/Effect/TextureParticle.h"
#include"../Game/Effect/SplineParticle.h"

#include"../Game/Tutorial/DirectX12MoviePlayer.h"
#include"../Game/Debug/ParticleWall.h"

#include"../KazLibrary/Collision/BoundingBox.h"
#include"../KazLibrary/Collision/BBDuringEquallyCoordinatePlace.h"
#include"../Game/CollisionDetection/CollisionDetectionOfMeshCircleAndParticle.h"
#include"../Game/Effect/InstanceMeshParticle.h"
#include"../KazLibrary/Buffer/CreateMeshBuffer.h"
#include"../KazLibrary/Render/GPUParticleRender.h"

#include"../Game/CollisionDetection/InstanceMeshCollision.h"

#include"../Game/CollisionDetection/GPUMeshAndSphereHitBox.h"

class DebugMeshParticleScene :public SceneBase
{
public:
	DebugMeshParticleScene();
	~DebugMeshParticleScene();

	void Init();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	KazMath::Vec3<float> debugCameraMove;
	KazMath::Vec3<float> eyePos, targetPos;
	KazMath::Vec2<float> angle;

	CameraWork cameraWork;

	static const int MESH_MAX_NUM = 6;
	std::array<std::unique_ptr<MeshParticleEmitter>, MESH_MAX_NUM> meshEmitter;
	std::array<std::string, MESH_MAX_NUM>filePass;

	int meshIndex, prevMeshIndex;
	bool cpuCheckParticleFlag;
	bool gpuCheckParticleFlag;
	bool textureParticleFlag;
	bool splineParticleFlag;
	bool perlinNoizeFlag;
	bool cpuCheckTriangleAndSphereHitBoxFlag;
	bool cpuCheckMeshParticleAndSphereHitBoxFlag;
	bool gpuCheckHitBoxFlag;
	bool drawInstanceMeshParticleFlag;
	bool drawMeshHitBoxFlag;
	bool drawGridFlag;


	bool deadParticleFlag, prevDeadParticleFlag;

	std::unique_ptr<DeadParticle> deadParticle;

	//デバック用--------------------------------------------
	//三角形描画
	std::array<KazMath::Vec3<float>, 3>triangelPosArray;
	std::array<LineRender, 3>triangelLine;
	std::array<LineRender, 3>clossTriangelLine;

	//パーティクルの数
	static const int PARTICLE_MAX_NUM = 1000;
	std::array<BoxPolygonRender, PARTICLE_MAX_NUM>particle;
	int drawParticleNum;
	int particleBias;//パーティクルをエッジ周辺に偏よらせる値
	static const int PARTICLE_MAX_BIAS = 100;//パーティクルをエッジ周辺に偏よらせる値

	//計算
	bool initFlag;
	BackGroundForDebug debugDraw;

	float CalucurateTriangleArea(KazMath::Vec3<float> P0, KazMath::Vec3<float> P1, KazMath::Vec3<float> P2)
	{
		KazMath::Vec3<float> p0p1Vec = P1 - P0;
		KazMath::Vec3<float> p1p2Vec = P2 - P1;
		KazMath::Vec3<float> r = p0p1Vec.Cross(p1p2Vec);
		return r.z / 2.0f;
	}

	float CalucurateUVW(KazMath::Vec3<float> P0, KazMath::Vec3<float> P1, KazMath::Vec3<float> ATTACK_POINT, float TRIANGLE_AREA)
	{
		KazMath::Vec3<float> p0p1Vec = ATTACK_POINT - P0;
		KazMath::Vec3<float> p1p2Vec = ATTACK_POINT - P1;
		KazMath::Vec3<float> r = p0p1Vec.Cross(p1p2Vec);
		float area = r.z / 2.0f;
		float rate = area / TRIANGLE_AREA;
		return rate;
	}
	//デバック用--------------------------------------------

	//GPUでのパーティクル--------------------------------------------
	DirectX::XMMATRIX motherMat;
	FbxModelRender model;
	//GPUでのパーティクル--------------------------------------------



	std::unique_ptr<GameRenderTarget>rendertarget;
	Sprite2DRender mainRenderTarget;


	//テクスチャをコンピュートシェーダーで読み込む--------------------------------------------
	std::unique_ptr<TextureParticle> texParticle;
	RESOURCE_HANDLE handle;
	KazMath::Transform3D texTransform;
	DirectX::XMMATRIX texMotherMat;
	//テクスチャをコンピュートシェーダーで読み込む--------------------------------------------

	//スプラインパーティクル--------------------------------------------
	std::unique_ptr<SplineParticle> splineParticle;
	std::vector<KazMath::Vec3<float>>limitPos;
	std::array<BoxPolygonRender, 6> controlPointR;
	bool initSplineFlag;

	std::vector<VertexUv>GetPlaneData(RESOURCE_HANDLE HANDLE)
	{
		std::array<Vertex, 4>vertices;
		std::array<USHORT, 6> indices;
		indices = KazRenderHelper::InitIndciesForPlanePolygon();
		KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, { 0.5f,0.5f });
		KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);

		KazMath::Vec2<int>lTexSize
		(
			static_cast<int>(TextureResourceMgr::Instance()->GetTextureSize(HANDLE).Width),
			static_cast<int>(TextureResourceMgr::Instance()->GetTextureSize(HANDLE).Height)
		);

		KazMath::Vec2<float> lLeftUp, lRightDown;
		lLeftUp = { 0.0f,0.0f };
		lRightDown = { 1.0f,1.0f };
		//サイズ変更
		std::array<KazMath::Vec2<float>, 4>lVert;
		lVert = KazRenderHelper::ChangePlaneScale(lLeftUp, lRightDown, { 1.0f,1.0f }, { 0.5f,0.5f }, lTexSize);


		std::array<int, 6>indiData;
		indiData[0] = 0;
		indiData[1] = 1;
		indiData[2] = 2;
		indiData[3] = 1;
		indiData[4] = 3;
		indiData[5] = 2;


		std::vector<VertexUv>lVertArray;

		for (int i = 0; i < indices.size(); ++i)
		{
			int index = indiData[i];
			VertexUv lData;
			lData.pos = { lVert[index].ConvertXMFLOAT2().x,lVert[index].ConvertXMFLOAT2().y,0.0f };
			lData.uv = vertices[index].uv;
			lVertArray.push_back(lData);
		}

		lVertArray[0].uv = { 1.0f,0.0f };
		lVertArray[1].uv = { 0.0f,0.0f };
		lVertArray[2].uv = { 0.0f,1.0f };
		lVertArray[3].uv = { 1.0f,0.0f };
		lVertArray[4].uv = { 0.0f,1.0f };
		lVertArray[5].uv = { 1.0f,1.0f };

		return lVertArray;
	}
	//スプラインパーティクル--------------------------------------------



	//パーリンノイズの確認--------------------------------------------
	float PerlinNoize(const KazMath::Vec2<float> &UV, int SEED);

	KazMath::Vec3<float>PNoise(const KazMath::Vec3<float> &POS)
	{
		KazMath::Vec3<float>vec =
		{
			PerlinNoize({POS.x,POS.y,},1),
			PerlinNoize({POS.x,POS.y,},1),
			PerlinNoize({POS.x,POS.y,},1)
		};
		return vec;
	}

	KazMath::Vec3<float>SamplePotential(const KazMath::Vec3<float> &POS)
	{
		return PNoise(POS / 10.0f);
	}

	KazMath::Vec3<float> CurlNoise(const KazMath::Vec3<float> &POS, int SEED)
	{
		const float e = 0.0009765625;
		const float e2 = 2.0 * e;
		const float invE2 = 1.0 / e2;
		int s = SEED;
		s = 0;

		const KazMath::Vec3<float> dx = KazMath::Vec3<float>(e, 0.0, 0.0);
		const KazMath::Vec3<float> dy = KazMath::Vec3<float>(0.0, e, 0.0);
		const KazMath::Vec3<float> dz = KazMath::Vec3<float>(0.0, 0.0, e);

		KazMath::Vec3<float> pos = POS;

		KazMath::Vec3<float> p_x0 = SamplePotential(pos - dx);
		KazMath::Vec3<float> p_x1 = SamplePotential(pos + dx);
		KazMath::Vec3<float> p_y0 = SamplePotential(pos - dy);
		KazMath::Vec3<float> p_y1 = SamplePotential(pos + dy);
		KazMath::Vec3<float> p_z0 = SamplePotential(pos - dz);
		KazMath::Vec3<float> p_z1 = SamplePotential(pos + dz);

		float x = (p_y1.z - p_y0.z) - (p_z1.y - p_z0.y);
		float y = (p_z1.x - p_z0.x) - (p_x1.z - p_x0.z);
		float z = (p_x1.y - p_x0.y) - (p_y1.x - p_y0.x);

		KazMath::Vec3<float>result(x, y, z);
		result *= invE2;
		return result;
	}


	float Frac(float LEN)
	{
		float lM;
		if (LEN < 0.1f)
		{
			return LEN;
		}
		else
		{
			lM = Frac(LEN - 1.0f);
			return LEN * lM;
		}
	}

	float Larp(float a, float b, float t)
	{
		return a + t * (b - a);
	}
	//パーリンノイズの確認--------------------------------------------

	RESOURCE_HANDLE modelHandle;
	//BackGroundForDebug debug;


	//ボロノイ領域による当たり判定の確認--------------------------------------------
	KazMath::Vec3<float>ClosestPoint(KazMath::Vec3<float>POINT_POS, KazMath::Vec3<float>TRIANGLE_A_POS, KazMath::Vec3<float>TRIANGLE_B_POS, KazMath::Vec3<float>TRIANGLE_C_POS);

	std::array<KazMath::Vec3<float>, 3>hitBoxTrianglePosArray;
	KazMath::Vec3<float> pointPos;
	float pointRadius;

	std::array<LineRender, 3>hitBoxTriangelLine;
	BoxPolygonRender pointPosR;
	BoxPolygonRender closestPointPosR;
	//ボロノイ領域による当たり判定の確認--------------------------------------------

	//GPU上でのメッシュとパーティクルの判定--------------------------------------------
	std::unique_ptr<GPUCollisionDetectionOfMeshHitBoxAndSphere> collision;
	std::unique_ptr<ParticleWall> particleWall;
	KazMath::Transform3D meshTransform;
	DirectX::XMMATRIX meshMat;
	//GPU上でのメッシュとパーティクルの判定--------------------------------------------


	//CPU上でのメッシュとパーティクルの判定--------------------------------------------
	struct TriangleLineData
	{
		KazMath::Vec3<float>startPos;
		KazMath::Vec3<float>endPos;
	};

	static const int PARTICLE_HITBOX_NUM = 20;
	void InitMeshParticle(const KazMath::Vec3<float> &BASE_POS, std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>> &PARTICLE_DATA, std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>> &BASE_PARTICLE_DATA);
	void SetParticle(std::array<TriangleLineData, 3>LINE_ARRAY_POS, std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM> &PARTICLE_DATA, std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM> &BASE_PARTICLE_DATA);
	void CollisionDetection(std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>> &PARTICLE_DATA, std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12> &LARP_POS, const KazMath::Vec3<float> &MESH_POS, const KazMath::Vec3<float> &BLOCK_POS);

	std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>>particleHitBox;
	std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>>baseParticleHitBox;
	std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>>particleHitBox2;
	std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>>baseParticleHitBox2;
	std::vector<std::array<int, PARTICLE_HITBOX_NUM>>particleAngle;
	std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12>particleLarpPos;
	std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12>particleLarpPos2;
	std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12>particleLarpPos3;
	std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12>particleLarpPos4;
	std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12>blockParticleLarpPos;
	std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12>blockParticleLarpPos2;
	KazMath::Vec3<float>meshPos, mesh2Pos, prevMeshPos;

	bool initParticleFlag;
	float larpVel;

	KazMath::Vec3<float>blockPos, blockPos2;

	LineRenderPtr vec1, vec2;
	float angleParticle;
	bool drawParticleFlag;

	FbxModelRender circleModelR, circleModelR2;
	FbxModelRenderPtr modelHitBox;

	//GPU上での歪み判定
	KazMath::Vec3<float>collisionPos;
	KazMath::Vec3<float>collisionPos2;
	KazMath::Vec3<float>collisionPos3;
	std::unique_ptr<InstanceMeshCollision> meshCollision;
	std::array<KazMath::Vec3<float>, 2> meshCollisionPos;
	std::array<DirectX::XMMATRIX, 2> meshCollisionMat;
	BoxPolygonRenderPtr minPos, maxPos;

	std::array<DirectX::XMMATRIX, 10> enemyModelMat;
	DirectX::XMMATRIX sphereModelMat;
	std::array<DirectX::XMMATRIX, 10> summonModelMat;
	//CPU上でのメッシュとパーティクルの判定--------------------------------------------

	std::array<KazMath::Vec3<float>, 36>GetSquareVertData(const KazMath::Vec3<float> &BASE_POS);

	std::vector<const ColorData *>colorArrayData;
	std::vector<Sphere*>hitBoxArray;
	Sphere s1, s2, s3;
	ColorData color1, color2;

	std::unique_ptr<InstanceMeshParticle> meshParticle;
	std::unique_ptr<GPUParticleRender> particleRender;

	CreateMeshBuffer sphereModel;
	CreateMeshBuffer enemyModel;
	CreateMeshBuffer summonModel;
	CreateMeshBuffer cubeModel;

	float al = 0.5f;
};

