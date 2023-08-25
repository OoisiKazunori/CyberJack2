#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"
#include"../KazLibrary/Animation/AnimationInRaytracing.h"

class VirusEnemy:public IEnemy
{
public:
	VirusEnemy(int arg_moveID, float arg_moveIDparam);

	void Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

private:

	DrawFuncData::DrawCallData m_model;
	KazMath::Vec3<float> m_prevPlayerPos;
	KazMath::Vec3<float> m_initPos;

	//ステータス
	enum STATUS {
		APPEAR,
		STAY,
		EXIT,
		DEAD,
	}m_status;

	//出現に関するイージングタイマー
	float m_appearEasingTimer;
	const float APPEAR_EASING_TIMER = 40.0f;


	//プレイヤーの正面ベクトルからどの程度ずらした位置に敵を出現させるかの値。
	float m_fromAroundAngle;	//場所を変えるときのイージングに使う。
	float m_aroundAngle;
	const float ADD_AROUND_ANGLE = 3.14f / 6.0f;
	const float AROUND_R = 20.0f;

	//STAY中の更新処理
	float m_stopTimer;	//場所を移動させるまでの遅延
	const float STOP_TIMER = 30.0f;
	float m_moveTimer;	//移動にかかる時間
	const float MOVE_TIMER = 30.0f;
	const float MOVE_ROTATE = 90.0f;	//移動する際に傾く量。
	bool m_isMove;

	//死亡演出用
	float m_gravity;
	const float GRAVITY = 0.05f;

	//自動的に消えるまでのタイマー
	int m_exitTimer;
	const int EXIT_TIMER = 420;

	//HP
	int m_hp;
	int m_prevhp;

	int debugTimer = 0;

	const float VIRUS_SCALE = 2.0f;

	//衝撃波
	float m_shockWaveTimer;
	const float SHOCK_WAVE_TIMER = 60.0f;
	const float SHOCK_WAVE_RAIDUS = 100.0f;


	//プレイヤーを中心とした半径どのくらいの位置に敵を出すか。
	const float SPAWN_R = 50.0f;


	DirectX::XMMATRIX m_motherMat;
	float m_alpha;

	AnimationInRaytracing m_computeAnimation;
	std::shared_ptr<ModelAnimator> m_animation;
	std::shared_ptr<ModelInfomation>m_modelData;
};

