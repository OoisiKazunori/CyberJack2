#pragma once
#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

class ButterflyEnemy :public IEnemy
{
public:
	ButterflyEnemy(int arg_moveID, float arg_moveIDparam);

	void Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

private:

	DrawFuncData::DrawCallData m_model;
	KazMath::Vec3<float> m_prevPlayerPos;

	//ステータス
	enum STATUS {
		APPEAR,
		STAY,
		EXIT,
		DEAD,
	}m_status;

	float m_prevAroundAngle;
	float m_aroundAngle;	//ちょうちょを出す位置

	//各ステータスを変えるアングル。
	const float START_ANGLE = DirectX::XM_PI / 2.0f;
	const float STAY_START_ANGLE = DirectX::XM_PI / 8.0f;
	const float STAY_FINISH_ANGLE = -0.1f;
	const float FINISH_ANGLE = -DirectX::XM_PI / 2.0f;

	//出現、退出中に使用する変数
	const float APPEAR_EXIT_AROUND_ANGLE = DirectX::XM_PI / 150.0f;
	const float STAY_AROUND_ANGLE = DirectX::XM_PI / 2000.0f;
	float m_addAroundAngle;
	float m_angleX;
	float m_angleYEasingTimer;
	const float ANGLEY_EASING_TIMER = 90.0f;
	float m_angleXEasingTimer;
	const float ANGLEX_EASING_TIMER = 80.0f;

	//死亡時に奥側にふっとばす値
	const float DEAD_MOVE_SPEED_Z = 3.0f;
	float m_deadMoveSpeedZ;

	DirectX::XMVECTOR m_postureQ;	//蝶の姿勢

	//蝶のスケール
	const float MODEL_SCALE = 80.0f;

	void RotationMatrixToEuler(const DirectX::XMMATRIX& R, double& roll, double& pitch, double& yaw)
	{
		roll = atan2(R.r[2].m128_f32[1], R.r[2].m128_f32[2]);
		pitch = asin(-R.r[2].m128_f32[0]);
		yaw = atan2(R.r[1].m128_f32[0], R.r[0].m128_f32[0]);
	}


	int debugTimer = 0;

	//HP
	int m_hp;
	int m_prevhp;

	//自動的に消えるまでのタイマー
	int m_exitTimer;
	const int EXIT_TIMER = 540;


	//プレイヤーを中心とした半径どのくらいの位置に敵を出すか。
	const float SPAWN_R = 10.0f;

};

