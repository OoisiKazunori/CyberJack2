#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Raytracing/BlasVector.h"

class Player
{
public:
	Player();
	void Init(const KazMath::Vec3<float> &POS, bool DRAW_UI_FLAG = true, bool APPEAR_FLAG = false);
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);
	
	void ForceAnim(int PATTERN)
	{
		switch (PATTERN)
		{
		case 0:
			forceCameraRate = 0.0f;
			break;
		case 1:
			forceCameraRate = 0.0f;
			break;
		case 2:
			forceCameraRate = -0.5f;
			break;
		case 3:
			forceCameraRate = 0.5f;
			break;
		default:
			break;
		}
	};

	void Hit();

	bool IsAlive();
	KazMath::Vec3<float> prevPos;
	KazMath::Vec3<float> pos;
	KazMath::Vec2<float> cameraRate;
	KazMath::Transform3D m_transform;
private:

	enum { LEFT, RIGHT, HEAD };

	short hp,prevHp;
	int coolTimer;
	static const int COOL_MAX_TIME;
	bool coolTimeFlag;
	bool redFlag;
	bool drawHpFlag;


	int damageSoundHandle;


	bool leftFlag;
	bool rightFlag;
	FbxTime totalTime;

	KazMath::Vec4<float> m_emissive;

	int larpTime;
	KazMath::Vec3<float>minScale;
	KazMath::Vec3<float>adjPos;
	KazMath::Vec3<float>adjRota;
	float sinTimer;

	float forceCameraRate;

	//std::array<DrawFuncData::DrawCallData, 1> m_playerModel;
};

