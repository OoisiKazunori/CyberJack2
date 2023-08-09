#include "LotusLeafEnemy.h"
#include "../KazLibrary/Easing/easing.h"

LotusLeafEnemy::LotusLeafEnemy()
{
	m_model = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Enemy/LotusLeaf/", "LotusLeaf.gltf"));
}

void LotusLeafEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "LotusLeaf");
	m_playerTransform = arg_playerTransform;
	//�o�������������߂�B
	const float HIGH_PRECISION = 1000.0f;	//���������ׂ������邽�߂̒萔�B
	m_spawnAngle = KazMath::Rand<float>(-SPAWN_ANGLE_RANGE * HIGH_PRECISION, SPAWN_ANGLE_RANGE * HIGH_PRECISION) / HIGH_PRECISION;
	//��U�v���C���[�̐^���ɐ�������B
	m_transform.pos = arg_playerTransform->pos;
	m_transform.pos.y = -100.0f;
	m_prevPlayerPos = m_playerTransform->pos;

	m_appearEasingTimer = 0;
	m_breathScaleChangeTimer = 0;

	debugTimer = 0;

}

void LotusLeafEnemy::Finalize()
{
}

void LotusLeafEnemy::Update()
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

	//���̃N�H�[�^�j�I�����X�ɗ�������]������B
	Vec3<float> upVec = TransformVector3({ 0,1,0 }, playerQ);
	playerQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(upVec), m_spawnAngle));
	//���ʃx�N�g�������߂�B
	Vec3<float> frontVec = TransformVector3({ 0,0,1 }, playerQ);

	//��Ԑ�̍��W
	Vec3<float> basePos = m_playerTransform->pos + frontVec * SPAWN_R;
	basePos.y = 5.0f;

	//��Ԍ��̍��W
	Vec3<float> fromPos = basePos;
	fromPos.y = -40.0f;

	
	++debugTimer;
	if (180 == debugTimer) {
		m_status = DEAD;
		m_deadEffectVel = m_playerTransform->pos - m_prevPlayerPos;
		m_deadEffectVelStorage = m_playerTransform->pos - m_prevPlayerPos;
		m_deadEffectVelStorage *= 1.0f;
	}

	//�X�e�[�^�X�ɂ���ď����𕪂���B
	switch (m_status)
	{
	case LotusLeafEnemy::APPEAR:
	{
		//�o���̃^�C�}�[���X�V�B
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER);

		//���W���Ԃ���B
		float posEasingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_appearEasingTimer / APPEAR_EASING_TIMER);
		m_transform.pos = fromPos + (basePos - fromPos) * posEasingValue;

		//�ċz���Ă���ۂ������邽�߂̃X�P�[����ς���B
		m_breathScaleChangeTimer += BREATH_SCALECHANGE_TIMER;
		float scale = sinf(m_breathScaleChangeTimer) * BREATH_CHANGESCALE;
		m_transform.scale = m_playerTransform->scale + Vec3<float>(scale, scale, scale);

		//���łɏ㉺�ɂ��ړ�������B
		float UPDOWN = 1.0f;
		m_transform.pos.y += sinf(m_breathScaleChangeTimer) * UPDOWN;

		//�o�����I�������ҋ@��Ԃ�
		if (APPEAR_EASING_TIMER <= m_appearEasingTimer) {
			m_status = STAY;
		}
	}
	break;
	case LotusLeafEnemy::STAY:
	{

		m_transform.pos = basePos;

		//�ċz���Ă���ۂ������邽�߂̃X�P�[����ς���B
		m_breathScaleChangeTimer += BREATH_SCALECHANGE_TIMER;
		float scale = sinf(m_breathScaleChangeTimer) * BREATH_CHANGESCALE;
		m_transform.scale = m_playerTransform->scale + Vec3<float>(scale, scale, scale);

		//���łɏ㉺�ɂ��ړ�������B
		float UPDOWN = 1.0f;
		m_transform.pos.y += sinf(m_breathScaleChangeTimer) * UPDOWN;

	}
	break;
	case LotusLeafEnemy::DEAD:
	{

		//���S���o�p�̈ړ��ʂ��܂��c���Ă�����
		m_deadEffectVelStorage -= m_deadEffectVelStorage / 2.0f;
		m_deadEffectVel += m_deadEffectVelStorage;

		m_transform.pos += m_deadEffectVel;
		m_transform.pos.y -= 0.5f;
		m_deadEffectVel -= m_deadEffectVel / 10.0f;
		m_transform.rotation.y += 6.0f;
	}
	break;
	default:
		break;
	}
	m_transform.rotation.y += 3.0f;

	DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);

	//�v���C���[�̍��W��ۑ��B
	m_prevPlayerPos = m_playerTransform->pos;
}

void LotusLeafEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawModel(m_model, m_transform);
	arg_rasterize.ObjectRender(m_model);
	for (auto& index : m_model.m_raytracingData.m_blas) {
		arg_blasVec.Add(index, m_transform.GetMat());
	}
}

KazMath::Vec3<float> LotusLeafEnemy::GetVec3(DirectX::XMVECTOR arg_target)
{
	return KazMath::Vec3<float>(arg_target.m128_f32[0], arg_target.m128_f32[1], arg_target.m128_f32[2]);
}

DirectX::XMVECTOR LotusLeafEnemy::GetXMVECTOR(KazMath::Vec3<float> arg_target)
{
	return DirectX::XMVECTOR{ arg_target.x, arg_target.y, arg_target.z, 0.0f };
}

KazMath::Vec3<float> LotusLeafEnemy::TransformVector3(KazMath::Vec3<float> arg_target, DirectX::XMVECTOR arg_quaternion)
{
	DirectX::XMVECTOR transformVec = DirectX::XMVector3Transform(DirectX::XMVectorSet(arg_target.x, arg_target.y, arg_target.z, 0), DirectX::XMMatrixRotationQuaternion(arg_quaternion));
	return GetVec3(transformVec);
}
