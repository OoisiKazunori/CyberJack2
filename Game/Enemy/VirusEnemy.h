#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IEnemy.h"
#include"../KazLibrary/Render/MeshParticleRender.h"
#include"../KazLibrary/Animation/AnimationInRaytracing.h"
#include"../Game/Effect/EnemyDeadParticle.h"
#include "../KazLibrary/Sound/SoundManager.h"

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
	KazMath::Vec3<float> m_initPos;	//�����n�_ ���������Ȃ�
	KazMath::Vec3<float> m_basePos; //�T�C���g�œ������Ƃ��̒��S�_ �����n�_������B

	//�X�e�[�^�X
	enum STATUS {
		APPEAR,
		STAY,
		EXIT,
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
	float m_gravity;
	const float GRAVITY = 0.05f;

	//�����I�ɏ�����܂ł̃^�C�}�[
	int m_exitTimer;
	const int EXIT_TIMER = 420;

	//HP
	int m_hp;
	int m_prevhp;

	int debugTimer = 0;

	const float VIRUS_SCALE = 2.0f;

	//�Ռ��g
	float m_shockWaveTimer;
	const float SHOCK_WAVE_TIMER = 60.0f;
	const float SHOCK_WAVE_RAIDUS = 100.0f;

	const KazMath::Vec4<float> OUTLINE_COLOR = KazMath::Vec4<float>(0.35f, 0.91f, 0.55f, 1.0f);
	//const KazMath::Vec4<float> OUTLINE_COLOR = KazMath::Vec4<float>(0.95f, 0.91f, 0.95f, 1.0f);


	//�v���C���[�𒆐S�Ƃ������a�ǂ̂��炢�̈ʒu�ɓG���o�����B
	const float SPAWN_R = 50.0f;

	//���S�G�t�F�N�g���̏����x
	const float DEAD_EFFECT_VEL = 2.5f;


	DirectX::XMMATRIX m_motherMat;
	float m_alpha;

	//���S���̉�
	SoundData m_dispperSE;

	struct DeadEffectData {
		KazMath::Vec4<float> m_dissolve;
		KazMath::Vec4<float> m_outlineColor;
	}m_deadEffectData;

	AnimationInRaytracing m_computeAnimation;
	std::shared_ptr<ModelAnimator> m_animation;
	std::shared_ptr<ModelInfomation>m_modelData;

	std::unique_ptr<MeshParticleRender>m_meshParticleRender;
	std::unique_ptr<EnemyDeadParticle>m_deadParticle;
	std::unique_ptr<EnemyDeadParticle>m_knockBackParticle;
	bool m_initDeadParticleFlag;


};

