#include "ButterflyEnemy.h"
#include "../KazLibrary/Easing/easing.h"
#include"../KazLibrary/Render/DrawFunc.h"

ButterflyEnemy::ButterflyEnemy()
{
	m_model = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Enemy/Butterfly/", "Butterfly.gltf"));
}

void ButterflyEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "Butterfly");
	m_playerTransform = arg_playerTransform;
	//�o�������������߂�B
	const float HIGH_PRECISION = 1000.0f;	//���������ׂ������邽�߂̒萔�B
	m_aroundAngle = START_ANGLE;
	m_addAroundAngle = 0;
	m_prevAroundAngle = m_aroundAngle;
	m_angleX = DirectX::XM_PI;

	m_postureQ = DirectX::XMQuaternionIdentity();

	m_status = APPEAR;

	debugTimer = 0;
}

void ButterflyEnemy::Finalize()
{
}

void ButterflyEnemy::Update()
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

	//���݂̊p�x�ɂ���Ďp���𓮂����B
	auto nowPosutreQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 1, 0), playerQ)), m_aroundAngle));

	//���݂̊p�x�ɂ���Ďp���𓮂����B
	auto prevPosutreQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 1, 0), playerQ)), m_prevAroundAngle));
	//�o���ʒu������B
	auto prevPos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), prevPosutreQ) * SPAWN_R;


	////�����ړ����Ă�������𐳖ʂƂ����p�������߂�B
	//movedVec = m_transform.pos - prevPos;
	////�����Ă�����p�����X�V�B�����Ă��Ȃ��������΂��l�ɂȂ邽�߁B
	//auto m_moveQ = DirectX::XMQuaternionIdentity();
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
	//		m_moveQ = DirectX::XMQuaternionRotationMatrix(cameraMatWorld);

	//	}

	//}


	//++debugTimer;
	//if (240 == debugTimer) {
	//	m_status = DEAD;

	//	//���S���ɂ��������ɑO�ɐi�܂��邽�߂̌v�Z�B�ȉ��O�s���E�������Ɏ����čs���Ă��������B
	//	m_deadEffectVel = m_playerTransform->pos - m_prevPlayerPos;
	//	m_deadEffectVelStorage = m_playerTransform->pos - m_prevPlayerPos;
	//	m_deadEffectVelStorage *= 2.0f;
	//}

	//�p�x���ς��O�ɕۑ��B
	m_prevAroundAngle = m_aroundAngle;

	switch (m_status)
	{
	case ButterflyEnemy::APPEAR:
	{
		//�o���ʒu������B
		m_transform.pos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), nowPosutreQ) * SPAWN_R;

		//��]�Ɋւ��鏈��
		m_postureQ = DirectX::XMQuaternionRotationAxis(GetXMVECTOR({ 1,0,0 }), m_angleX);
		//�I�C���[�p�ɒ����B
		DirectX::XMVECTOR rotate, scale, position;
		DirectX::XMMatrixDecompose(&scale, &rotate, &position, DirectX::XMMatrixRotationQuaternion(m_postureQ));
		m_transform.rotation = KazMath::Vec3<float>(DirectX::XMConvertToDegrees(rotate.m128_f32[0]), DirectX::XMConvertToDegrees(rotate.m128_f32[1]), DirectX::XMConvertToDegrees(rotate.m128_f32[2]));

		//�ʒu�Ɋւ��鏈��
		m_aroundAngle -= m_addAroundAngle;
		m_addAroundAngle += (APPEAR_EXIT_AROUND_ANGLE - m_addAroundAngle) / 10.0f;
		if (m_aroundAngle < STAY_START_ANGLE) {
			m_status = STAY;
		}

	}
	break;
	case ButterflyEnemy::STAY:
	{


		//�o���ʒu������B
		m_transform.pos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), nowPosutreQ) * SPAWN_R;

		//��]�Ɋւ��鏈��
		m_angleX -= m_angleX / 10.0f;
		m_postureQ = DirectX::XMQuaternionRotationAxis(GetXMVECTOR({ 1,0,0 }), m_angleX);
		//�I�C���[�p�ɒ����B
		DirectX::XMVECTOR rotate, scale, position;
		DirectX::XMMatrixDecompose(&scale, &rotate, &position, DirectX::XMMatrixRotationQuaternion(m_postureQ));
		m_transform.rotation = KazMath::Vec3<float>(DirectX::XMConvertToDegrees(rotate.m128_f32[0]), DirectX::XMConvertToDegrees(rotate.m128_f32[1]), DirectX::XMConvertToDegrees(rotate.m128_f32[2]));

		//�ʒu�Ɋւ��鏈��
		m_aroundAngle -= m_addAroundAngle;
		m_addAroundAngle += (STAY_AROUND_ANGLE - m_addAroundAngle) / 10.0f;
		if (m_aroundAngle < STAY_FINISH_ANGLE) {
			m_status = DEAD;
			//���S���ɂ��������ɑO�ɐi�܂��邽�߂̌v�Z�B�ȉ��O�s���E�������Ɏ����čs���Ă��������B
			m_deadEffectVel = m_playerTransform->pos - m_prevPlayerPos;
			m_deadEffectVelStorage = m_playerTransform->pos - m_prevPlayerPos;
			m_deadEffectVelStorage *= 0.05f;
		}

	}
	break;
	case ButterflyEnemy::EXIT:
	{

	}
	break;
	case ButterflyEnemy::DEAD:
	{

		//���S���o�p�̈ړ��ʂ��܂��c���Ă�����
		m_deadEffectVelStorage -= m_deadEffectVelStorage / 15.0f;
		m_deadEffectVel += m_deadEffectVelStorage;

		m_transform.pos += m_deadEffectVel;
		m_transform.pos.y -= 0.3f;
		m_deadEffectVel -= m_deadEffectVel / 10.0f;
		m_transform.rotation.x += 3.0f;
		m_transform.rotation.z += 3.0f;
	}
	break;
	default:
		break;
	}

	m_transform.scale = m_playerTransform->scale;

	DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);

	//�v���C���[�̍��W��ۑ��B
	m_prevPlayerPos = m_playerTransform->pos;
}

void ButterflyEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawModel(m_model, m_transform);
	arg_rasterize.ObjectRender(m_model);
	for (auto& index : m_model.m_raytracingData.m_blas) {
		arg_blasVec.Add(index, m_transform.GetMat());
	}
}
