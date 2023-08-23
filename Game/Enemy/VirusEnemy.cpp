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

	//アニメーション対応
	m_animation = std::make_shared<ModelAnimator>(m_modelData);
	m_computeAnimation.GenerateBuffer(*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0]);
	m_spawnTimer = 0;
	m_canSpawn = false;
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
	m_canLockOn = false;

	m_status = APPEAR;

	m_transform.pos = GENERATE_DATA.initPos;
	m_initPos = GENERATE_DATA.initPos;

	debugTimer = 0;
	m_exitTimer = 0;
	m_alpha;

	m_animation->Play("繧｢繝ｼ繝槭メ繝･繧｢Action", true, false);
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

	//死んでいたらリスポーンするまでのタイマーを更新
	m_canSpawn = false;
	if (!iEnemy_EnemyStatusData->oprationObjData->initFlag) {

		const int RESPAWN_TIMER = 120;
		++m_spawnTimer;
		if (RESPAWN_TIMER < m_spawnTimer) {
			m_spawnTimer = 0;
			m_canSpawn = true;
		}

	}

	//HPを保存。
	m_prevhp = m_hp;
	m_hp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;

	//HPが減った瞬間だったらスケールを変える。
	if (m_hp != m_prevhp) {

		//攻撃を食らったときのリアクション用
		const float DEAD_EFFECT_SCALE = 1.0f;
		m_transform.scale += KazMath::Vec3<float>(DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE);

	}


	//移動した方向をもとにプレイヤーの姿勢を求める。
	KazMath::Vec3<float> movedVec = m_playerTransform->pos - m_prevPlayerPos;
	//動いていたら姿勢を更新。動いていなかったらやばい値になるため。
	DirectX::XMVECTOR playerQ = DirectX::XMQuaternionIdentity();
	//if (0 < movedVec.Length()) {

	//	KazMath::Vec3<float> movedVecNormal = movedVec.GetNormal();

	//	//デフォルトの回転軸と移動した方向のベクトルが同じ値だったらデフォルトの回転軸の方向に移動しているってこと！
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

	//出現位置の中心点を決定。
	//Vec3<float> centerPos = m_playerTransform->pos + Vec3<float>(0, 0, 1) * SPAWN_R;
	//現在の角度によって姿勢を動かす。
	playerQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(TransformVector3(Vec3<float>(0, 0, 1), playerQ)), m_aroundAngle));


	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag && !m_isDead) {
		m_status = DEAD;

		m_isDead = true;
		iEnemy_EnemyStatusData->oprationObjData->initFlag = false;

		//攻撃を食らったときのリアクション用
		const float DEAD_EFFECT_SCALE = 1.0f;
		m_transform.scale += KazMath::Vec3<float>(DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE);

		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->m_isSeaEffect = true;

	}

	////自動的に消えるまでのタイマーを更新。
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
		//現在の位置を確定。
		//m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;
		m_moveTimer += 0.06f;
		m_transform.pos.y = m_initPos.y + sinf(m_moveTimer) * 1.0f;

		//出現のタイマーを更新。
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 10.0f, 0.0f, APPEAR_EASING_TIMER);
		++m_appearEasingTimer;

		//座標を補間する。
		float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER) / APPEAR_EASING_TIMER);
		m_transform.scale = KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) * easingValue;
		m_transform.rotation = Vec3<float>(0, 0, 360) * easingValue;

		//出現が終わったら待機状態へ
		if (APPEAR_EASING_TIMER + 10.0f <= m_appearEasingTimer) {
			m_status = STAY;
		}

	}
	break;
	case VirusEnemy::STAY:
	{

		//サイン波で良い感じに動かす用のタイマー。名前は以前使ってた処理のママ。
		m_stopTimer += 0.1f;
		m_moveTimer += 0.06f;

		m_transform.rotation.z = 360.0f + sinf(m_stopTimer) * 35.0f;
		m_transform.pos.y = m_initPos.y + sinf(m_moveTimer) * 1.0f;

		m_transform.scale += (KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) - m_transform.scale) / 5.0f;

		//現在の位置を確定。
		//m_transform.pos = centerPos + TransformVector3(Vec3<float>(0, 1, 0), playerQ) * AROUND_R;

		////動いている状態だったら
		//if (m_isMove) {

		//	//移動のタイマーを進める。
		//	m_moveTimer = std::clamp(m_moveTimer + 1.0f, 0.0f, MOVE_TIMER);

		//	//座標を補間する。
		//	float easingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_moveTimer / MOVE_TIMER);
		//	m_transform.rotation.z = MOVE_ROTATE - MOVE_ROTATE * easingValue;
		//	easingValue = EasingMaker(EasingType::Out, EaseInType::Exp, m_moveTimer / MOVE_TIMER);
		//	m_aroundAngle = m_fromAroundAngle + ADD_AROUND_ANGLE * easingValue;
		//	m_transform.scale += (m_playerTransform->scale - m_transform.scale) / 10.0;

		//	//出現が終わったらSTOP状態へ
		//	if (MOVE_TIMER <= m_moveTimer) {
		//		m_isMove = false;
		//		m_stopTimer = 0;
		//	}

		//}
		////動いていない状態だったら
		//else {

		//	//移動のタイマーを進める。
		//	m_stopTimer = std::clamp(m_stopTimer + 1.0f, 0.0f, STOP_TIMER);

		//	//座標を補間する。
		//	float easingValue = EasingMaker(EasingType::Out, EaseInType::Sine, m_stopTimer / STOP_TIMER);
		//	m_transform.scale = m_playerTransform->scale + easingValue * 30.0f;
		//	m_transform.rotation.z = MOVE_ROTATE * easingValue;

		//	//出現が終わったらMOVE状態へ
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

		////小さくして消す。
		//m_transform.scale.x = std::clamp(m_transform.scale.x - 2.0f, 0.0f, 100.0f);
		//m_transform.scale.y = std::clamp(m_transform.scale.y - 2.0f, 0.0f, 100.0f);
		//m_transform.scale.z = std::clamp(m_transform.scale.z - 2.0f, 0.0f, 100.0f);

		////回す
		//m_transform.rotation.z += 10.0f;

		////小さくなったら殺す。
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

	//プレイヤーの座標を保存。
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
