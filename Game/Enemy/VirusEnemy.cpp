#include "VirusEnemy.h"
#include "../KazLibrary/Easing/easing.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Effect/SeaEffect.h"

VirusEnemy::VirusEnemy(int arg_moveID, float arg_moveIDparam)
{
	m_modelData = ModelLoader::Instance()->Load("Resource/Enemy/Virus/", "virus_cur.gltf");
	m_model = DrawFuncData::SetDefferdRenderingModelAnimation(m_modelData);
	m_alpha = 1.0f;
	iEnemy_EnemyStatusData->fAlpha = &m_alpha;
	InitMeshPartilce("Resource/Enemy/Virus/", "virus_cur.gltf", &m_motherMat);

	moveID = arg_moveID;
	moveIDparam = arg_moveIDparam;

	//�A�j���[�V�����Ή�
	m_animation = std::make_shared<ModelAnimator>(m_modelData);
	m_computeAnimation.GenerateBuffer(*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0]);
	m_spawnTimer = 0;
	m_canSpawn = false;
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
	m_canLockOn = false;

	m_status = APPEAR;

	m_transform.pos = GENERATE_DATA.initPos;
	m_initPos = GENERATE_DATA.initPos;

	debugTimer = 0;
	m_exitTimer = 0;
	m_alpha;

	m_animation->Play("アーマチュアAction", true, false);
	m_gravity = 0;

	m_hp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;
	m_prevhp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;
	m_spawnTimer = 0;
	m_canSpawn = false;
}

void VirusEnemy::Finalize()
{
}

void VirusEnemy::Update()
{
	using namespace KazMath;

	//����ł����烊�X�|�[������܂ł̃^�C�}�[���X�V
	m_canSpawn = false;
	if (!iEnemy_EnemyStatusData->oprationObjData->initFlag) {

		const int RESPAWN_TIMER = 120;
		++m_spawnTimer;
		if (RESPAWN_TIMER < m_spawnTimer) {
			m_spawnTimer = 0;
			m_canSpawn = true;
		}

	}

	//HP��ۑ��B
	m_prevhp = m_hp;
	m_hp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;

	//HP���������u�Ԃ�������X�P�[����ς���B
	if (m_hp != m_prevhp) {

		//�U����H������Ƃ��̃��A�N�V�����p
		const float DEAD_EFFECT_SCALE = 1.0f;
		m_transform.scale += KazMath::Vec3<float>(DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE);

	}


	//�ړ��������������ƂɃv���C���[�̎p�������߂�B
	KazMath::Vec3<float> movedVec = m_playerTransform->pos - m_prevPlayerPos;
	//�����Ă�����p�����X�V�B�����Ă��Ȃ��������΂��l�ɂȂ邽�߁B
	DirectX::XMVECTOR playerQ = DirectX::XMQuaternionIdentity();
	//if (0 < movedVec.Length()) {

	//	KazMath::Vec3<float> movedVecNormal = movedVec.GetNormal();

	//	//�f�t�H���g�̉�]���ƈړ����������̃x�N�g���������l��������f�t�H���g�̉�]���̕����Ɉړ����Ă�����Ă��ƁI
	//	if (movedVecNormal.Dot(KazMath::Vec3<float>(0, 0, 1)) < 0.999f) {

	//		KazMath::Vec3<float> cameraAxisZ = movedVecNormal;
	//		KazMath::Vec3<float> cameraAxisY = KazMath::Vec3<float>(0, 1, 0);
	//		KazMath::Vec3<float> cameraAxisX = cameraAxisY.Cross(cameraAxisZ);
	//		cameraAxisY = cameraAxisZ.Cross(cameraAxisX);
	//		DirectX::XMMATRIX cameraMatWorld = DirectX::XMMatrixIdentity();
	//		cameraMatWorld.r[0] = { cameraAxisX.x, cameraAxisX.y, cameraAxisX.z, 0.0f };
	//		cameraMatWorld.r[1] = { cameraAxisY.x, cameraAxisY.y, cameraAxisY.z, 0.0f };
	//		cameraMatWorld.r[2] = { cameraAxisZ.x, cameraAxisZ.y, cameraAxisZ.z, 0.0f };
	//		playerQ = DirectX::XMQuaternionRotationMatrix(cameraMatWorld);

	//	}

	//}

	//�o���ʒu�̒��S�_������B
	//Vec3<float> centerPos = m_playerTransform->pos + Vec3<float>(0, 0, 1) * SPAWN_R;
	//���݂̊p�x�ɂ���Ďp���𓮂����B
	playerQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 0, 1), playerQ)), m_aroundAngle));


	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag && !m_isDead) {
		m_status = DEAD;

		m_isDead = true;
		iEnemy_EnemyStatusData->oprationObjData->initFlag = false;

		//�U����H������Ƃ��̃��A�N�V�����p
		const float DEAD_EFFECT_SCALE = 1.0f;
		m_transform.scale += KazMath::Vec3<float>(DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE);

		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->m_isSeaEffect = true;

	}

	////�����I�ɏ�����܂ł̃^�C�}�[���X�V�B
	//++m_exitTimer;
	//if (EXIT_TIMER <= m_exitTimer) {
	//	m_status = EXIT;
	//}

	m_canLockOn = false;
	iEnemy_EnemyStatusData->curlNozieFlag = false;
	switch (m_status)
	{
	case VirusEnemy::APPEAR:
	{
		//���݂̈ʒu���m��B
		//m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;
		m_moveTimer += 0.06f;
		m_transform.pos.y = m_initPos.y + sinf(m_moveTimer) * 1.0f;

		//�o���̃^�C�}�[���X�V�B
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 10.0f, 0.0f, APPEAR_EASING_TIMER);
		++m_appearEasingTimer;

		//���W���Ԃ���B
		float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER) / APPEAR_EASING_TIMER);
		m_transform.scale = KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) * easingValue;
		m_transform.rotation = Vec3<float>(0, 0, 360) * easingValue;

		//�o�����I�������ҋ@��Ԃ�
		if (APPEAR_EASING_TIMER + 10.0f <= m_appearEasingTimer) {
			m_status = STAY;
		}

	}
	break;
	case VirusEnemy::STAY:
	{

		//�T�C���g�ŗǂ������ɓ������p�̃^�C�}�[�B���O�͈ȑO�g���Ă������̃}�}�B
		m_stopTimer += 0.1f;
		m_moveTimer += 0.06f;

		m_transform.rotation.z = 360.0f + sinf(m_stopTimer) * 35.0f;
		m_transform.pos.y = m_initPos.y + sinf(m_moveTimer) * 1.0f;

		m_transform.scale += (KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) - m_transform.scale) / 5.0f;

		//���݂̈ʒu���m��B
		//m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;

		////�����Ă����Ԃ�������
		//if (m_isMove) {

		//	//�ړ��̃^�C�}�[��i�߂�B
		//	m_moveTimer = std::clamp(m_moveTimer + 1.0f, 0.0f, MOVE_TIMER);

		//	//���W���Ԃ���B
		//	float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_moveTimer / MOVE_TIMER);
		//	m_transform.rotation.z = MOVE_ROTATE - MOVE_ROTATE * easingValue;
		//	easingValue = EasingMaker(EasingType::Out, EaseInType::Exp, m_moveTimer / MOVE_TIMER);
		//	m_aroundAngle = m_fromAroundAngle + ADD_AROUND_ANGLE * easingValue;
		//	m_transform.scale += (m_playerTransform->scale - m_transform.scale) / 10.0;

		//	//�o�����I�������STOP��Ԃ�
		//	if (MOVE_TIMER <= m_moveTimer) {
		//		m_isMove = false;
		//		m_stopTimer = 0;
		//	}

		//}
		////�����Ă��Ȃ���Ԃ�������
		//else {

		//	//�ړ��̃^�C�}�[��i�߂�B
		//	m_stopTimer = std::clamp(m_stopTimer + 1.0f, 0.0f, STOP_TIMER);

		//	//���W���Ԃ���B
		//	float easingValue = EasingMaker(EasingType::Out, EaseInType::Sine, m_stopTimer / STOP_TIMER);
		//	m_transform.scale = m_playerTransform->scale + easingValue * 30.0f;
		//	m_transform.rotation.z = MOVE_ROTATE * easingValue;

		//	//�o�����I�������MOVE��Ԃ�
		//	if (STOP_TIMER <= m_stopTimer) {
		//		m_isMove = true;
		//		m_moveTimer = 0;
		//		m_fromAroundAngle = m_aroundAngle;
		//	}
		//}

		m_canLockOn = true;
	}
	break;
	case VirusEnemy::EXIT:
	{

		////���������ď����B
		//m_transform.scale.x = std::clamp(m_transform.scale.x - 2.0f, 0.0f, 100.0f);
		//m_transform.scale.y = std::clamp(m_transform.scale.y - 2.0f, 0.0f, 100.0f);
		//m_transform.scale.z = std::clamp(m_transform.scale.z - 2.0f, 0.0f, 100.0f);

		////��
		//m_transform.rotation.z += 10.0f;

		////�������Ȃ�����E���B
		//if (m_transform.scale.x <= 0) {

		//	m_isDead = true;
		//	iEnemy_EnemyStatusData->oprationObjData->initFlag = false;

		//}

	}
	break;
	case VirusEnemy::DEAD:
	{

		m_transform.scale += (KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) - m_transform.scale) / 5.0f;
		m_gravity += 0.06f;
		m_transform.pos.y -= m_gravity;
		m_transform.rotation.x += 3.0f;

		iEnemy_EnemyStatusData->curlNozieFlag = true;
	}
	break;
	default:
		break;
	}

	//DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);

	//�v���C���[�̍��W��ۑ��B
	m_prevPlayerPos = m_playerTransform->pos;

	iEnemy_EnemyStatusData->hitBox.center = &m_transform.pos;
	iEnemy_EnemyStatusData->hitBox.radius = 10.0f;

	m_motherMat = m_transform.GetMat();

	m_animation->Update(1.0f);
	/*m_computeAnimation.Compute(
		*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0],
		m_animation->GetBoneMatBuff(),
		m_transform.GetMat()
	);*/
}

void VirusEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawModel(m_model, m_transform, m_animation->GetBoneMatBuff());
	arg_rasterize.ObjectRender(m_model);
	for (auto& index : m_model.m_raytracingData.m_blas)
	{
		arg_blasVec.Add(index, m_transform.GetMat());
	}
}
