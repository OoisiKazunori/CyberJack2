#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../Game/Effect/RocketEffect.h"

class VirusEnemy:public IEnemy
{
public:
	VirusEnemy();

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
	const float APPEAR_EASING_TIMER = 40.0f;


	//�v���C���[�̐��ʃx�N�g������ǂ̒��x���炵���ʒu�ɓG���o�������邩�̒l�B
	float m_fromAroundAngle;	//�ꏊ��ς���Ƃ��̃C�[�W���O�Ɏg���B
	float m_aroundAngle;
	const float ADD_AROUND_ANGLE = 3.14f / 6.0f;
	const float AROUND_R = 20.0f;

	//STAY���̍X�V����
	float m_stopTimer;	//�ꏊ���ړ�������܂ł̒x��
	const float STOP_TIMER = 30.0f;
	float m_moveTimer;	//�ړ��ɂ����鎞��
	const float MOVE_TIMER = 30.0f;
	const float MOVE_ROTATE = 90.0f;	//�ړ�����ۂɌX���ʁB
	bool m_isMove;

	//���S���o�p
	KazMath::Vec3<float> m_deadEffectVel;
	KazMath::Vec3<float> m_deadEffectVelStorage;


	int debugTimer = 0;


	//�v���C���[�𒆐S�Ƃ������a�ǂ̂��炢�̈ʒu�ɓG���o�����B
	const float SPAWN_R = 50.0f;

};

