#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include<array>
#include<memory>
#include"../KazLibrary/Loader/MeshParticleLoader.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Raytracing/BlasVector.h"

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
	virtual void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec) = 0;

	void SetCamera(int CAMERA_INDEX)
	{
		cameraIndex = CAMERA_INDEX;
	};



	std::vector<InitMeshParticleData> particleArrrayData;


	std::array<bool,16> hitFlag;
	bool startFlag = false;
	Sprite3DRender vaporWaveSunRender;
	KazMath::Vec3<float> playerPos;
protected:
	int cameraIndex = 0;
	float objectSpeed;

private:
	std::array<std::unique_ptr<PolygonRender>, 4>backGround;
	std::unique_ptr<PolygonRender> topPolygon;
	std::unique_ptr<PolygonRender> buttomPolygon;

};

