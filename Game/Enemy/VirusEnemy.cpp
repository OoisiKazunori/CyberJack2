#include "VirusEnemy.h"
#include "../KazLibrary/Easing/easing.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Effect/SeaEffect.h"

VirusEnemy::VirusEnemy(int arg_moveID, float arg_moveIDparam)
{
	m_model = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Enemy/Virus/", "Virus.gltf"));
	moveID = arg_moveID;
	moveIDparam = arg_moveIDparam;
}

void VirusEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "Virus");
	m_playerTransform = arg_playerTransform;
	//�o�������������߂�B
	const float HIGH_PRECISION = 1000.0f;	//���������ׂ������邽�߂̒萔�B
	m_aroundAngle = KazMath::Rand<float>(-DirectX::XM_2PI * HIGH_PRECISION, DirectX::XM_2PI * HIGH_PRECISION) / HIGH_PRECISION;
	m_fromAroundAngle = m_aroundAngle;

	m_appearEasingTimer = 0;
	m_stopTimer = 0;
	m_moveTimer = 0;
	m_isMove = false;
	m_isDead = false;

	m_status = APPEAR;

	debugTimer = 0;
}

void VirusEnemy::Finalize()
{
}

void VirusEnemy::Update()
{
	using namespace KazMath;

	//�ړ��������������ƂɃv���C���[�̎p�������߂�B
	KazMath::Vec3<float> movedVec = m_playerTransform->pos - m_prevPlayerPos;
	//�����Ă�����p�����X�V�B�����Ă��Ȃ��������΂��l�ɂȂ邽�߁B
	DirectX::XMVECTOR playerQ = DirectX::XMQuaternionIdentity();
	if (0 < movedVec.Length()) {

		KazMath::Vec3<float> movedVecNormal = movedVec.GetNormal();

		//�f�t�H���g�̉�]���ƈړ����������̃x�N�g���������l��������f�t�H���g�̉�]���̕����Ɉړ����Ă�����Ă��ƁI
		if (movedVecNormal.Dot(KazMath::Vec3<float>(0, 0, 1)) < 0.999f) {

			KazMath::Vec3<float> cameraAxisZ = movedVecNormal;
			KazMath::Vec3<float> cameraAxisY = KazMath::Vec3<float>(0, 1, 0);
			KazMath::Vec3<float> cameraAxisX = cameraAxisY.Cross(cameraAxisZ);
			cameraAxisY = cameraAxisZ.Cross(cameraAxisX);
			DirectX::XMMATRIX cameraMatWorld = DirectX::XMMatrixIdentity();
			cameraMatWorld.r[0] = { cameraAxisX.x, cameraAxisX.y, cameraAxisX.z, 0.0f };
			cameraMatWorld.r[1] = { cameraAxisY.x, cameraAxisY.y, cameraAxisY.z, 0.0f };
			cameraMatWorld.r[2] = { cameraAxisZ.x, cameraAxisZ.y, cameraAxisZ.z, 0.0f };
			playerQ = DirectX::XMQuaternionRotationMatrix(cameraMatWorld);

		}

	}

	//�o���ʒu�̒��S�_������B
	Vec3<float> centerPos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), playerQ) * SPAWN_R;
	//���݂̊p�x�ɂ���Ďp���𓮂����B
	playerQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 0, 1), playerQ)), m_aroundAngle));


	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag && !m_isDead) {
		m_status = DEAD;
		//���S���ɂ��������ɑO�ɐi�܂��邽�߂̌v�Z�B�ȉ��O�s���E�������Ɏ����čs���Ă��������B
		m_deadEffectVel = m_playerTransform->pos - m_prevPlayerPos;
		m_deadEffectVelStorage = m_playerTransform->pos - m_prevPlayerPos;
		m_deadEffectVelStorage *= 2.0f;
		m_isDead = true;
		//iEnemy_EnemyStatusData->oprationObjData->initFlag = false;

		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->m_isSeaEffect = true;
		
	}

	switch (m_status)
	{
	case VirusEnemy::APPEAR:
	{
		//���݂̈ʒu���m��B
		m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;

		//�o���̃^�C�}�[���X�V�B
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER);

		//���W���Ԃ���B
		float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_appearEasingTimer / APPEAR_EASING_TIMER);
		m_transform.scale = m_playerTransform->scale * easingValue;
		m_transform.rotation = Vec3<float>(0, 0, 360) * easingValue;

		//�o�����I�������ҋ@��Ԃ�
		if (APPEAR_EASING_TIMER <= m_appearEasingTimer) {
			m_status = STAY;
		}

	}
	break;
	case VirusEnemy::STAY:
	{
		//���݂̈ʒu���m��B
		m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;

		//�����Ă����Ԃ�������
		if (m_isMove) {

			//�ړ��̃^�C�}�[��i�߂�B
			m_moveTimer = std::clamp(m_moveTimer + 1.0f, 0.0f, MOVE_TIMER);

			//���W���Ԃ���B
			float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_moveTimer / MOVE_TIMER);
			m_transform.rotation.z = MOVE_ROTATE - MOVE_ROTATE * easingValue;
			easingValue = EasingMaker(EasingType::Out, EaseInType::Exp, m_moveTimer / MOVE_TIMER);
			m_aroundAngle = m_fromAroundAngle + ADD_AROUND_ANGLE * easingValue;
			m_transform.scale += (m_playerTransform->scale - m_transform.scale) / 10.0;

			//�o�����I�������STOP��Ԃ�
			if (MOVE_TIMER <= m_moveTimer) {
				m_isMove = false;
				m_stopTimer = 0;
			}

		}
		//�����Ă��Ȃ���Ԃ�������
		else {

			//�ړ��̃^�C�}�[��i�߂�B
			m_stopTimer = std::clamp(m_stopTimer + 1.0f, 0.0f, STOP_TIMER);

			//���W���Ԃ���B
			float easingValue = EasingMaker(EasingType::Out, EaseInType::Sine, m_stopTimer / STOP_TIMER);
			m_transform.scale = m_playerTransform->scale + easingValue * 30.0f;
			m_transform.rotation.z = MOVE_ROTATE * easingValue;

			//�o�����I�������MOVE��Ԃ�
			if (STOP_TIMER <= m_stopTimer) {
				m_isMove = true;
				m_moveTimer = 0;
				m_fromAroundAngle = m_aroundAngle;
			}
		}
	}
	break;
	case VirusEnemy::DEAD:
	{

		//���S���o�p�̈ړ��ʂ��܂��c���Ă�����
		m_deadEffectVelStorage -= m_deadEffectVelStorage / 2.0f;
		m_deadEffectVel += m_deadEffectVelStorage;

		m_transform.pos += m_deadEffectVel;
		m_transform.pos.y -= 0.8f;
		m_deadEffectVel -= m_deadEffectVel / 10.0f;
		m_transform.rotation.x += 3.0f;
	}
		break;
	default:
		break;
	}

	DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);

	//�v���C���[�̍��W��ۑ��B
	m_prevPlayerPos = m_playerTransform->pos;

	iEnemy_EnemyStatusData->hitBox.center = &m_transform.pos;
	iEnemy_EnemyStatusData->hitBox.radius = 10.0f;
}

void VirusEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawModel(m_model, m_transform);
	arg_rasterize.ObjectRender(m_model);
	for (auto& index : m_model.m_raytracingData.m_blas) {
		arg_blasVec.Add(index, m_transform.GetMat());
	}
}
