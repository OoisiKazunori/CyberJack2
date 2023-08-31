#include "PlayerShotEffect.h"
#include <Render/DrawFunc.h>
#include "../Game/Effect/StopMgr.h"

PlayerShotEffect::PlayerShotEffect()
{

	m_vertexBufferHandle = -1;
	//m_model = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/ShotEffect/", "ShotEffect.gltf"));

	auto playerModel = *ModelLoader::Instance()->Load("Resource/ShotEffect/", "ShotEffect.gltf");
	auto pipeline = DrawFuncData::GetAnimationModelBloomShader();
	m_model = DrawFuncData::SetDrawGLTFAnimationIndexMaterialInRayTracingBloomData(playerModel, pipeline);

	m_shotSE = SoundManager::Instance()->SoundLoadWave("Resource/Sound/Razer.wav");
	m_shotSE.volume = 0.01f;
	m_hitSE = SoundManager::Instance()->SoundLoadWave("Resource/Sound/hit.wav");
	m_hitSE.volume = 0.01f;

}

void PlayerShotEffect::Init()
{

	m_frame = 0;

}

void PlayerShotEffect::Generate(const KazMath::Vec3<float>* arg_refPlayerPos, shared_ptr<IEnemy> arg_refEnemy, std::array<std::shared_ptr<IEnemy>, 7> arg_refOtherEnemy)
{
	//制御点を決めるベクトルを求める。
	m_controlPointVec = KazMath::Vec3<float>(KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R), KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R), KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R));

	m_frame = 0;
	m_refPlayerPos = arg_refPlayerPos;
	m_refEnemy = arg_refEnemy;
	m_otherEnemy = arg_refOtherEnemy;
	m_isActive = true;
	m_isFinish = false;
	m_transform.scale = KazMath::Vec3<float>(1.0f, 1.0f, 1.0f);

	//Line描画用の処理
	m_splineRailPosArray.resize(POINT_COUNT);
	m_splineRailPosArray.front() = *m_refPlayerPos;
	m_splineRailPosArray.back() = m_refEnemy->m_transform.pos;
	if (m_vertexBufferHandle != -1)
	{
		VertexBufferMgr::Instance()->ReleaseVeretexBuffer(m_vertexBufferHandle);
	}
	VertexGenerateData generateData(m_splineRailPosArray.data(), sizeof(DirectX::XMFLOAT3), m_splineRailPosArray.size(), sizeof(m_splineRailPosArray[0]));
	m_vertexBufferHandle = VertexBufferMgr::Instance()->GenerateBuffer(generateData, false);
	m_splineDrawCall = DrawFuncData::SetLine(m_vertexBufferHandle);

	SoundManager::Instance()->SoundPlayerWave(m_shotSE, 0);

}

void PlayerShotEffect::Update(std::array<bool, 16>& arg_hitArray, int* arg_hitNum)
{

	//ベジエの終点に達していたら
	if (m_isFinish) {

		m_transform.pos += m_finishVel;

	}
	//まだベジエの終点に達していなかったら
	else {

		m_prevPos = m_transform.pos;

		//現在のフレームのタイマーを更新。
		++m_frame;

		//始点と終点の値を決める。
		std::array<KazMath::Vec3<float>, 4> controlPoints;
		controlPoints.front() = *m_refPlayerPos - KazMath::Vec3<float>(0, 0, 10.0f);
		controlPoints.back() = m_refEnemy->m_transform.pos;

		//制御点の場所を決める。
		controlPoints[1] = controlPoints[0] + m_controlPointVec;
		controlPoints[2] = controlPoints[3] + m_controlPointVec;

		//座標を求める。
		m_transform.pos = EvaluateBezierCurve(controlPoints, static_cast<float>(m_frame) / static_cast<float>(EFFECT_FRAME));

		//移動した方向をもとにプレイヤーの姿勢を求める。
		KazMath::Vec3<float> movedVec = m_transform.pos - m_prevPos;
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

		m_transform.quaternion = playerQ;

		//ベジエ曲線上の点を計算する。
		for (int index = 0; index < POINT_COUNT; ++index) {

			//このIndexの時間
			float time = static_cast<float>(index) / static_cast<float>(POINT_COUNT);

			m_splineRailPosArray[index] = EvaluateBezierCurve(controlPoints, time);

		}
		m_splineRailPosArray.back() = controlPoints[3];

		//現在の時間を01で求める。
		float nowTime = static_cast<float>(m_frame) / static_cast<float>(EFFECT_FRAME);
		//現在の時間以下のIndexの頂点をなくす。
		int vertexDeadline = std::clamp(static_cast<int>(nowTime * POINT_COUNT), 0, POINT_COUNT - 1);
		for (int index = 0; index < vertexDeadline; ++index) {
			m_splineRailPosArray[index] = m_splineRailPosArray[vertexDeadline];
		}

	}

	const float SHOCK_WAVE_VEL = 0.3f;
	//一定フレーム経過したら処理を終わらせる。
	if (EFFECT_FRAME <= m_frame && !m_isFinish) {

		if (0 < m_refEnemy->iOperationData.rockOnNum) {

			//m_refEnemy->Hit();
			StopMgr::Instance()->HitStopEnemy();
			m_refEnemy->Dead(&m_prevPos);

			for (int i = 0; i < arg_hitArray.size(); ++i)
			{
				if (!arg_hitArray[i])
				{
					arg_hitArray[i] = true;
					break;
				}
			}

			//周りの敵も反動で動かす。
			for (auto& index : m_otherEnemy) {

				index->SetShockWaveVel(KazMath::Vec3<float>(index->m_transform.pos - m_refEnemy->m_transform.pos).GetNormal() * SHOCK_WAVE_VEL);

			}
			SoundManager::Instance()->SoundPlayerWave(m_hitSE, 0);

		}
		else {

			//周りの敵も反動で動かす。
			StopMgr::Instance()->HitStopEnemy();
			m_refEnemy->Dead(&m_prevPos);

			for (auto& index : m_otherEnemy) {

				index->SetShockWaveVel(KazMath::Vec3<float>(index->m_transform.pos - m_refEnemy->m_transform.pos).GetNormal() * SHOCK_WAVE_VEL);

			}

			for (int i = 0; i < arg_hitArray.size(); ++i)
			{
				if (!arg_hitArray[i])
				{
					arg_hitArray[i] = true;
					break;
				}
			}
			SoundManager::Instance()->SoundPlayerWave(m_hitSE, 0);

		}

		m_isFinish = true;
		m_finishVel = (m_transform.pos - m_prevPos);
	}

	if (m_isFinish) {

		m_transform.scale.z -= m_transform.scale.z / 2.0f;

		if (m_transform.scale.z < 0.01f) {

			m_isActive = false;

		}

	}

	//DrawFunc::Test(m_model, m_transform, DrawFunc::NONE);

	m_emissive.x = 1.0f;
	m_emissive.y = 1.0f;
	m_emissive.z = 1.0f;
	m_emissive.a = 1;
	m_model.extraBufferArray.back().bufferWrapper->TransData(&m_emissive, sizeof(DirectX::XMFLOAT4));

}

void PlayerShotEffect::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{

	if (!m_isActive) {
		m_transform.pos.y = -1000;
	}

	//DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);
	DrawFunc::DrawModel(m_model, m_transform);
	arg_rasterize.ObjectRender(m_model);
	//for (auto& index : m_model.m_raytracingData.m_blas) {
	//	arg_blasVec.Add(index, m_transform.GetMat());
	//}

	//DrawFunc::DrawLine(m_splineDrawCall, m_splineRailPosArray, m_vertexBufferHandle);
	//arg_rasterize.ObjectRender(m_splineDrawCall);

}

KazMath::Vec3<float> PlayerShotEffect::EvaluateBezierCurve(const std::array<KazMath::Vec3<float>, 4>& arg_controlPoints, float arg_t) {

	float u = 1.0f - arg_t;
	float u2 = u * u;
	float t2 = arg_t * arg_t;

	//ベジェ曲線の評価式
	KazMath::Vec3<float> point = (arg_controlPoints[0] * (u2 * u)) +
		(arg_controlPoints[1] * (3.0f * u2 * arg_t)) +
		(arg_controlPoints[2] * (3.0f * u * t2)) +
		(arg_controlPoints[3] * (t2 * arg_t));

	return point;
}