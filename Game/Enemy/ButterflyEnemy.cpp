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
	//出現させる一を決める。
	const float HIGH_PRECISION = 1000.0f;	//乱数をより細かくするための定数。
	m_aroundAngle = START_ANGLE;
	m_addAroundAngle = 0;
	m_prevAroundAngle = m_aroundAngle;
	m_angleX = DirectX::XM_PIDIV2 * 0.9f;
	m_angleYEasingTimer = 0;
	m_angleXEasingTimer = 0;

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

	//移動した方向をもとにプレイヤーの姿勢を求める。
	KazMath::Vec3<float> movedVec = m_playerTransform->pos - m_prevPlayerPos;
	//動いていたら姿勢を更新。動いていなかったらやばい値になるため。
	DirectX::XMVECTOR playerQ = DirectX::XMQuaternionIdentity();
	if (0 < movedVec.Length()) {

		KazMath::Vec3<float> movedVecNormal = movedVec.GetNormal();

		//デフォルトの回転軸と移動した方向のベクトルが同じ値だったらデフォルトの回転軸の方向に移動しているってこと！
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

	//現在の角度によって姿勢を動かす。
	auto nowPosutreQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 1, 0), playerQ)), m_aroundAngle));

	//現在の角度によって姿勢を動かす。
	auto prevPosutreQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 1, 0), playerQ)), m_prevAroundAngle));
	//出現位置を決定。
	auto prevPos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), prevPosutreQ) * SPAWN_R;


	//蝶が移動している方向を正面とした姿勢を求める。
	movedVec = m_transform.pos - prevPos;
	//動いていたら姿勢を更新。動いていなかったらやばい値になるため。
	auto m_moveQ = DirectX::XMQuaternionIdentity();
	if (0 < movedVec.Length()) {

		KazMath::Vec3<float> movedVecNormal = movedVec.GetNormal();

		//デフォルトの回転軸と移動した方向のベクトルが同じ値だったらデフォルトの回転軸の方向に移動しているってこと！
		if (movedVecNormal.Dot(KazMath::Vec3<float>(0, 0, 1)) < 0.999f) {

			KazMath::Vec3<float> cameraAxisZ = movedVecNormal;
			KazMath::Vec3<float> cameraAxisY = KazMath::Vec3<float>(0, 1, 0);
			KazMath::Vec3<float> cameraAxisX = cameraAxisY.Cross(cameraAxisZ);
			cameraAxisY = cameraAxisZ.Cross(cameraAxisX);
			DirectX::XMMATRIX cameraMatWorld = DirectX::XMMatrixIdentity();
			cameraMatWorld.r[0] = { cameraAxisX.x, cameraAxisX.y, cameraAxisX.z, 0.0f };
			cameraMatWorld.r[1] = { cameraAxisY.x, cameraAxisY.y, cameraAxisY.z, 0.0f };
			cameraMatWorld.r[2] = { cameraAxisZ.x, cameraAxisZ.y, cameraAxisZ.z, 0.0f };
			m_moveQ = DirectX::XMQuaternionRotationMatrix(cameraMatWorld);

		}

	}


	//++debugTimer;
	//if (240 == debugTimer) {
	//	m_status = DEAD;

	//	//死亡時にいい感じに前に進ませるための計算。以下三行を殺す処理に持って行ってください。
	//	m_deadEffectVel = m_playerTransform->pos - m_prevPlayerPos;
	//	m_deadEffectVelStorage = m_playerTransform->pos - m_prevPlayerPos;
	//	m_deadEffectVelStorage *= 2.0f;
	//}

	//角度が変わる前に保存。
	m_prevAroundAngle = m_aroundAngle;

	//Y軸回転のイージングを更新する。
	m_angleYEasingTimer = std::clamp(m_angleYEasingTimer + 1, 0.0f, ANGLEY_EASING_TIMER);
	float easingAmount = EasingMaker(EasingType::Out, EaseInType::Sine, m_angleYEasingTimer / ANGLEY_EASING_TIMER);
	m_angleXEasingTimer = std::clamp(m_angleXEasingTimer + 1, 0.0f, ANGLEX_EASING_TIMER);
	float easingXAmount = EasingMaker(EasingType::In, EaseInType::Cubic, m_angleXEasingTimer / ANGLEX_EASING_TIMER);

	switch (m_status)
	{
	case ButterflyEnemy::APPEAR:
	{
		//出現位置を決定。
		m_transform.pos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), nowPosutreQ) * SPAWN_R;
		m_transform.pos.y += 5.0f;

		//現在の動いた角度を01の割合で出す。
		float maxMoveAngle = (START_ANGLE - STAY_FINISH_ANGLE);
		float moveAngleRate = fabs(m_aroundAngle - START_ANGLE) / maxMoveAngle;

		m_transform.pos.y += sinf(moveAngleRate * DirectX::XM_PI - DirectX::XM_PI) * 2.0f;

		//回転に関する処理
		m_postureQ = DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3({ 1,0,0 }, m_moveQ)), m_angleX - m_angleX * easingXAmount);
		m_postureQ = DirectX::XMQuaternionMultiply(m_postureQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3({ 0,1,0 }, m_postureQ)), easingAmount * (DirectX::XM_2PI * 2.0f)));
		m_transform.quaternion = m_postureQ;

		//位置に関する処理
		m_aroundAngle -= m_addAroundAngle;
		m_addAroundAngle += (APPEAR_EXIT_AROUND_ANGLE - m_addAroundAngle) / 10.0f;
		if (m_aroundAngle < STAY_START_ANGLE) {
			m_status = STAY;
		}

	}
	break;
	case ButterflyEnemy::STAY:
	{


		//出現位置を決定。
		m_transform.pos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), nowPosutreQ) * SPAWN_R;
		m_transform.pos.y += 5.0f;

		//現在の動いた角度を01の割合で出す。
		float maxMoveAngle = (START_ANGLE - STAY_FINISH_ANGLE);
		float moveAngleRate = fabs(m_aroundAngle - START_ANGLE) / maxMoveAngle;

		m_transform.pos.y += sinf(moveAngleRate * DirectX::XM_PI - DirectX::XM_PI) * 2.0f;

		//回転に関する処理
		m_postureQ = DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3({ 1,0,0 }, m_moveQ)), m_angleX - m_angleX * easingXAmount);
		m_postureQ = DirectX::XMQuaternionMultiply(m_postureQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3({ 0,1,0 }, m_postureQ)), easingAmount * (DirectX::XM_2PI * 2.0f)));
		m_transform.quaternion = m_postureQ;

		//位置に関する処理
		m_aroundAngle -= m_addAroundAngle;
		m_addAroundAngle += (STAY_AROUND_ANGLE - m_addAroundAngle) / 10.0f;
		if (m_aroundAngle < STAY_FINISH_ANGLE) {
			m_status = DEAD;
			//死亡時にいい感じに前に進ませるための計算。以下三行を殺す処理に持って行ってください。
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

		//死亡演出用の移動量がまだ残っていたら
		m_deadEffectVelStorage -= m_deadEffectVelStorage / 15.0f;
		m_deadEffectVel += m_deadEffectVelStorage;

		m_transform.pos += m_deadEffectVel;
		m_transform.pos.y -= 0.6f;
		m_transform.pos.x -= 0.2f;
		m_deadEffectVel -= m_deadEffectVel / 10.0f;
		m_transform.quaternion = DirectX::XMQuaternionMultiply(m_transform.quaternion, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3({ 1,0,0 }, m_transform.quaternion)), 0.1f));
		m_transform.quaternion = DirectX::XMQuaternionMultiply(m_transform.quaternion, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3({ 0,0,1 }, m_transform.quaternion)), 0.1f));
	}
	break;
	default:
		break;
	}

	m_transform.scale = m_playerTransform->scale;

	DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);

	//プレイヤーの座標を保存。
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
