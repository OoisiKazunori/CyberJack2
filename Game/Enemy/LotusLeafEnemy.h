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

	//�X�e�[�^�X
	enum STATUS {
		APPEAR,
		STAY,
		DEAD,
	}m_status;

	//�o���Ɋւ���C�[�W���O�^�C�}�[
	float m_appearEasingTimer;
	const float APPEAR_EASING_TIMER = 42.0f;

	//�ċz���Ă���ۂ��X�P�[����ς��邽�߂̃^�C�}�[
	float m_breathScaleChangeTimer;	//�ċz���ۂ������������邽�߂̃^�C�}�[Sin�Ŏg���B
	const float BREATH_SCALECHANGE_TIMER = 0.16f;
	const float BREATH_CHANGESCALE = 6.0f;

	//�v���C���[�̐��ʃx�N�g������ǂ̒��x���炵���ʒu�ɓG���o�������邩�̒l�B
	float m_spawnAngle;
	const float SPAWN_ANGLE_RANGE = 3.14f / 3.0f;

	//���S���o�p
	KazMath::Vec3<float> m_deadEffectVel;
	KazMath::Vec3<float> m_deadEffectVelStorage;


	int debugTimer = 0;


	//�v���C���[�𒆐S�Ƃ������a�ǂ̂��炢�̈ʒu�ɓG���o�����B
	const float SPAWN_R = 50.0f;

};

