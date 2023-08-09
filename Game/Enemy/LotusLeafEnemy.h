#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"
#include"../KazLibrary/Render/DrawFunc.h"

class LotusLeafEnemy :public IEnemy
{
public:
	LotusLeafEnemy();

	void Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

private:

	DrawFuncData::DrawCallData m_model;
	KazMath::Transform3D m_transform;

};

