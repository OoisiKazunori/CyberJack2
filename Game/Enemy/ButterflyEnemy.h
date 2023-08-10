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
	const float STAY_START_ANGLE = DirectX::XM_PI / 10.0f;
	const float STAY_FINISH_ANGLE = -DirectX::XM_PI / 10.0f;
	const float FINISH_ANGLE = -DirectX::XM_PI / 2.0f;

	//�o���A�ޏo���Ɏg�p����ϐ�
	const float APPEAR_EXIT_AROUND_ANGLE = DirectX::XM_PI / 150.0f;
	const float STAY_AROUND_ANGLE = DirectX::XM_PI / 800.0f;
	float m_addAroundAngle;
	float m_angleX;

	DirectX::XMVECTOR m_postureQ;	//���̎p��
	

	//���S���o�p
	KazMath::Vec3<float> m_deadEffectVel;
	KazMath::Vec3<float> m_deadEffectVelStorage;


	int debugTimer = 0;


	//�v���C���[�𒆐S�Ƃ������a�ǂ̂��炢�̈ʒu�ɓG���o�����B
	const float SPAWN_R = 15.0f;

};

