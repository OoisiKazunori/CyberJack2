#pragma once
#include "PlayerShotEffect.h"
#include "../KazLibrary/Helper/ISinglton.h"
#include <array>
#include <memory>
#include <Math/KazMath.h>
#include "../Interface/IEnemy.h"

class PlayerShotEffectMgr : public ISingleton<PlayerShotEffectMgr> {

private:

	static const int EFFECT_NUM = 20;
	std::array<PlayerShotEffect, EFFECT_NUM> m_effects;

public:

	PlayerShotEffectMgr();
	void Init();
	void Generate(const KazMath::Vec3<float>* arg_refPlayerPoint, shared_ptr<IEnemy> arg_refEnemy);
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

};