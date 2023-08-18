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
	//出現させる一を決める。
	const float HIGH_PRECISION = 1000.0f;	//乱数をより細かくするための定数。
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

	//出現位置の中心点を決定。
	Vec3<float> centerPos = m_playerTransform->pos + TransformVector3(Vec3<float>(0, 0, 1), playerQ) * SPAWN_R;
	//現在の角度によって姿勢を動かす。
	playerQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 0, 1), playerQ)), m_aroundAngle));


	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag && !m_isDead) {
		m_status = DEAD;
		//死亡時にいい感じに前に進ませるための計算。以下三行を殺す処理に持って行ってください。
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
		//現在の位置を確定。
		m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;

		//出現のタイマーを更新。
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER);

		//座標を補間する。
		float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_appearEasingTimer / APPEAR_EASING_TIMER);
		m_transform.scale = m_playerTransform->scale * easingValue;
		m_transform.rotation = Vec3<float>(0, 0, 360) * easingValue;

		//出現が終わったら待機状態へ
		if (APPEAR_EASING_TIMER <= m_appearEasingTimer) {
			m_status = STAY;
		}

	}
	break;
	case VirusEnemy::STAY:
	{
		//現在の位置を確定。
		m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;

		//動いている状態だったら
		if (m_isMove) {

			//移動のタイマーを進める。
			m_moveTimer = std::clamp(m_moveTimer + 1.0f, 0.0f, MOVE_TIMER);

			//座標を補間する。
			float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_moveTimer / MOVE_TIMER);
			m_transform.rotation.z = MOVE_ROTATE - MOVE_ROTATE * easingValue;
			easingValue = EasingMaker(EasingType::Out, EaseInType::Exp, m_moveTimer / MOVE_TIMER);
			m_aroundAngle = m_fromAroundAngle + ADD_AROUND_ANGLE * easingValue;
			m_transform.scale += (m_playerTransform->scale - m_transform.scale) / 10.0;

			//出現が終わったらSTOP状態へ
			if (MOVE_TIMER <= m_moveTimer) {
				m_isMove = false;
				m_stopTimer = 0;
			}

		}
		//動いていない状態だったら
		else {

			//移動のタイマーを進める。
			m_stopTimer = std::clamp(m_stopTimer + 1.0f, 0.0f, STOP_TIMER);

			//座標を補間する。
			float easingValue = EasingMaker(EasingType::Out, EaseInType::Sine, m_stopTimer / STOP_TIMER);
			m_transform.scale = m_playerTransform->scale + easingValue * 30.0f;
			m_transform.rotation.z = MOVE_ROTATE * easingValue;

			//出現が終わったらMOVE状態へ
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

		//死亡演出用の移動量がまだ残っていたら
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

	//プレイヤーの座標を保存。
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
