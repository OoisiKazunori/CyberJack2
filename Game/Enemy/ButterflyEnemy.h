#pragma once
#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

class ButterflyEnemy :public IEnemy
{
public:
	ButterflyEnemy();

	void Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG);
	void Finalize();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

private:

	DrawFuncData::DrawCallData m_model;
	KazMath::Vec3<float> m_prevPlayerPos;
	KazMath::Transform3D m_transform;

	//�X�e�[�^�X
	enum STATUS {
		APPEAR,
		STAY,
		EXIT,
		DEAD,
	}m_status;

	float m_prevAroundAngle;
	float m_aroundAngle;	//���傤������o���ʒu

	//�e�X�e�[�^�X��ς���A���O���B
	const float START_ANGLE = DirectX::XM_PI / 2.0f;
	const float STAY_START_ANGLE = DirectX::XM_PI / 8.0f;
	const float STAY_FINISH_ANGLE = -0.1f;
	const float FINISH_ANGLE = -DirectX::XM_PI / 2.0f;

	//�o���A�ޏo���Ɏg�p����ϐ�
	const float APPEAR_EXIT_AROUND_ANGLE = DirectX::XM_PI / 150.0f;
	const float STAY_AROUND_ANGLE = DirectX::XM_PI / 2000.0f;
	float m_addAroundAngle;
	float m_angleX;
	float m_angleYEasingTimer;
	const float ANGLEY_EASING_TIMER = 90.0f;
	float m_angleXEasingTimer;
	const float ANGLEX_EASING_TIMER = 80.0f;

	DirectX::XMVECTOR m_postureQ;	//���̎p��

	void RotationMatrixToEuler(const DirectX::XMMATRIX& R, double& roll, double& pitch, double& yaw)
	{
		roll = atan2(R.r[2].m128_f32[1], R.r[2].m128_f32[2]);
		pitch = asin(-R.r[2].m128_f32[0]);
		yaw = atan2(R.r[1].m128_f32[0], R.r[0].m128_f32[0]);
	}

	//���S���o�p
	KazMath::Vec3<float> m_deadEffectVel;
	KazMath::Vec3<float> m_deadEffectVelStorage;


	int debugTimer = 0;


	//�v���C���[�𒆐S�Ƃ������a�ǂ̂��炢�̈ʒu�ɓG���o�����B
	const float SPAWN_R = 15.0f;

};

