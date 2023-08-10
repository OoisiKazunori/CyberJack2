#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

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
	KazMath::Vec3<float> m_prevPlayerPos;
	KazMath::Transform3D m_transform;

	//ステータス
	enum STATUS {
		APPEAR,
		STAY,
		DEAD,
	}m_status;

	//出現に関するイージングタイマー
	float m_appearEasingTimer;
	const float APPEAR_EASING_TIMER = 42.0f;

	//呼吸してるっぽくスケールを変えるためのタイマー
	float m_breathScaleChangeTimer;	//呼吸っぽい挙動をさせるためのタイマーSinで使う。
	const float BREATH_SCALECHANGE_TIMER = 0.16f;
	const float BREATH_CHANGESCALE = 6.0f;

	//プレイヤーの正面ベクトルからどの程度ずらした位置に敵を出現させるかの値。
	float m_spawnAngle;
	const float SPAWN_ANGLE_RANGE = 3.14f / 3.0f;

	//死亡演出用
	KazMath::Vec3<float> m_deadEffectVel;
	KazMath::Vec3<float> m_deadEffectVelStorage;


	int debugTimer = 0;


	//プレイヤーを中心とした半径どのくらいの位置に敵を出すか。
	const float SPAWN_R = 50.0f;

};

