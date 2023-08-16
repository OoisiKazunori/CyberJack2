#pragma once
#include "PlayerShotEffect.h"
#include "../KazLibrary/Helper/ISinglton.h"
#include <array>
#include <memory>
#include <Math/KazMath.h>
#include "../Interface/IEnemy.h"

class PlayerShotEffect {

private:

	const KazMath::Vec3<float>* m_refPlayerPos;
	KazMath::Vec3<float> m_controlPointVec;
	const float CONTROL_POINT_R = 10.0f;
	static const int POINT_COUNT = 10;
	std::array<KazMath::Vec3<float>, POINT_COUNT> m_points;
	shared_ptr<IEnemy> m_refEnemy;
	int m_frame;
	const int EFFECT_FRAME = 2;

	//線の描画に必要な変数ら
	int m_vertexBufferHandle;//デバック用のスプライン曲線描画の頂点バッファ
	DrawFuncData::DrawCallData m_splineDrawCall;
	std::vector<KazMath::Vec3<float>>m_splineRailPosArray;
	

public:

	bool m_isActive;

public:

	PlayerShotEffect();

	void Init();

	void Generate(const KazMath::Vec3<float>* arg_refPlayerPoint, shared_ptr<IEnemy> arg_refEnemy);

	void Update();
	
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

};