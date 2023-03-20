#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include<array>
#include<memory>
#include"../Game/CollisionDetection/InstanceMeshCollision.h"
#include"../KazLibrary/Loader/MeshParticleLoader.h"

/// <summary>
/// ステージの抽象クラス
/// ステージ毎の描画を纏めてあります
/// </summary>
class IStage
{
public:
	IStage();

	virtual ~IStage();
	void InitBackGround(const KazMath::Color &START_COLOR, const KazMath::Color &END_COLOR, float SPACE, float HEIGHT);
	void DrawBackGround();

	virtual void Update() = 0;
	virtual void Draw() = 0;

	void SetCamera(int CAMERA_INDEX)
	{
		cameraIndex = CAMERA_INDEX;
	};



	std::vector<InitMeshCollisionData> collisionArrrayData;
	std::vector<InitMeshParticleData> particleArrrayData;
	std::vector<Sphere*> blockHitBox;


	bool hitFlag = false;
	bool startFlag = false;
	Sprite3DRender vaporWaveSunRender;
protected:
	int cameraIndex = 0;
	float objectSpeed;

private:
	std::array<std::unique_ptr<PolygonRender>, 4>backGround;
	std::unique_ptr<PolygonRender> topPolygon;
	std::unique_ptr<PolygonRender> buttomPolygon;

};

