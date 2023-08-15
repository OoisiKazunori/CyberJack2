#include "LotusLeafEnemy.h"
#include "../KazLibrary/Easing/easing.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Effect/SeaEffect.h"

LotusLeafEnemy::LotusLeafEnemy()
{
	m_model = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Enemy/LotusLeaf/", "LotusLeaf.gltf"));
}

void LotusLeafEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "LotusLeaf");
	m_playerTransform = arg_playerTransform;
	//出現させる一を決める。
	const float HIGH_PRECISION = 1000.0f;	//乱数をより細かくするための定数。
	m_spawnAngle = KazMath::Rand<float>(-SPAWN_ANGLE_RANGE * HIGH_PRECISION, SPAWN_ANGLE_RANGE * HIGH_PRECISION) / HIGH_PRECISION;
	//一旦プレイヤーの真下に生成する。
	m_transform.pos = arg_playerTransform->pos;
	m_transform.pos.y = -100.0f;
	m_prevPlayerPos = m_playerTransform->pos;
	m_isDead = false;

	m_appearEasingTimer = 0;
	m_breathScaleChangeTimer = 0;

	m_status = APPEAR;

	debugTimer = 0;

}

void LotusLeafEnemy::Finalize()
{
}

void LotusLeafEnemy::Update()
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

	//このクォータニオンを更に乱数分回転させる。
	Vec3<float> upVec = TransformVector3({ 0,1,0 }, playerQ);
	playerQ = DirectX::XMQuaternionMultiply(playerQ, DirectX::XMQuaternionRotationAxis(GetXMVECTOR(upVec), m_spawnAngle));
	//正面ベクトルを求める。
	Vec3<float> frontVec = TransformVector3({ 0,0,1 }, playerQ);

	//補間先の座標
	Vec3<float> basePos = m_playerTransform->pos + frontVec * SPAWN_R;
	basePos.y = 5.0f;

	//補間元の座標
	Vec3<float> fromPos = basePos;
	fromPos.y = -40.0f;

	
	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag && !m_isDead) {
		m_status = DEAD;

		//死亡時にいい感じに前に進ませるための計算。以下三行を殺す処理に持って行ってください。
		m_deadEffectVel = m_playerTransform->pos - m_prevPlayerPos;
		m_deadEffectVelStorage = m_playerTransform->pos - m_prevPlayerPos;
		m_deadEffectVelStorage *= 1.0f;
		m_isDead = true;
		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->m_isSeaEffect = true;
		//iEnemy_EnemyStatusData->oprationObjData->initFlag = false;
	}

	//ステータスによって処理を分ける。
	switch (m_status)
	{
	case LotusLeafEnemy::APPEAR:
	{
		//出現のタイマーを更新。
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER);

		//座標を補間する。
		float posEasingValue = EasingMaker(EasingType::Out, EaseInType::Back, m_appearEasingTimer / APPEAR_EASING_TIMER);
		m_transform.pos = fromPos + (basePos - fromPos) * posEasingValue;

		//呼吸してるっぽく見せるためのスケールを変える。
		m_breathScaleChangeTimer += BREATH_SCALECHANGE_TIMER;
		float scale = sinf(m_breathScaleChangeTimer) * BREATH_CHANGESCALE;
		m_transform.scale = m_playerTransform->scale + Vec3<float>(scale, scale, scale);

		//ついでに上下にも移動させる。
		float UPDOWN = 1.0f;
		m_transform.pos.y += sinf(m_breathScaleChangeTimer) * UPDOWN;

		//出現が終わったら待機状態へ
		if (APPEAR_EASING_TIMER <= m_appearEasingTimer) {
			m_status = STAY;
		}
	}
	break;
	case LotusLeafEnemy::STAY:
	{

		m_transform.pos = basePos;

		//呼吸してるっぽく見せるためのスケールを変える。
		m_breathScaleChangeTimer += BREATH_SCALECHANGE_TIMER;
		float scale = sinf(m_breathScaleChangeTimer) * BREATH_CHANGESCALE;
		m_transform.scale = m_playerTransform->scale + Vec3<float>(scale, scale, scale);

		//ついでに上下にも移動させる。
		float UPDOWN = 1.0f;
		m_transform.pos.y += sinf(m_breathScaleChangeTimer) * UPDOWN;

	}
	break;
	case LotusLeafEnemy::DEAD:
	{

		//死亡演出用の移動量がまだ残っていたら
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

	//プレイヤーの座標を保存。
	m_prevPlayerPos = m_playerTransform->pos;

	iEnemy_EnemyStatusData->hitBox.center = &m_transform.pos;
	iEnemy_EnemyStatusData->hitBox.radius = 10.0f;
}

void LotusLeafEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawModel(m_model, m_transform);
	arg_rasterize.ObjectRender(m_model);
	for (auto& index : m_model.m_raytracingData.m_blas) {
		arg_blasVec.Add(index, m_transform.GetMat());
	}
}


