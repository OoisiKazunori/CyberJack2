#include "VirusEnemy.h"
#include "../KazLibrary/Easing/easing.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Effect/SeaEffect.h"
#include"../Effect/ShockWave.h"
#include"../KazLibrary/Buffer/GBufferMgr.h"
#include"../Effect/EnemyDissolveParam.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Raytracing/Blas.h"

VirusEnemy::VirusEnemy(int arg_moveID, float arg_moveIDparam)
{
	m_modelData = ModelLoader::Instance()->Load("Resource/Enemy/Virus/", "virus_cur.gltf");
	m_model = DrawFuncData::SetDefferdRenderingModelAnimationDissolve(m_modelData);
	m_alpha = 1.0f;
	iEnemy_EnemyStatusData->fAlpha = &m_alpha;
	InitMeshPartilce("Resource/Enemy/Virus/", "virus_cur.gltf", &m_motherMat);

	moveID = arg_moveID;
	moveIDparam = arg_moveIDparam;

	//アニメーション対応
	m_animation = std::make_shared<ModelAnimator>(m_modelData);
	m_computeAnimation.GenerateBuffer(*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0]);
	m_spawnTimer = 0;

	//モデル受け渡し
	iEnemy_EnemyStatusData->meshParticleData[0]->meshParticleData.modelVertexBuffer = *VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0];
	iEnemy_EnemyStatusData->meshParticleData[0]->meshParticleData.modelVertexBuffer.elementNum = VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0]->elementNum;
	//メッシュパーティクルの表示
	m_meshParticleRender = std::make_unique<MeshParticleRender>(iEnemy_EnemyStatusData->meshParticleData[0]->meshParticleData);

	//SEをロード
	m_dispperSE = SoundManager::Instance()->SoundLoadWave("Resource/Sound/disapper.wav");
	m_dispperSE.volume = 0.002f;
	m_spawnSE = SoundManager::Instance()->SoundLoadWave("Resource/Sound/respawn.wav");
	m_spawnSE.volume = 0.003f;

}

void VirusEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& arg_generateData, bool arg_demoFlag)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "Virus");
	m_playerTransform = arg_playerTransform;

	m_appearEasingTimer = 0;
	m_moveSineTimer = 0;
	m_isDead = false;
	m_canLockOn = false;

	m_status = APPEAR;

	m_transform.pos = arg_generateData.initPos;
	m_initPos = arg_generateData.initPos;
	m_basePos = m_initPos;

	m_animation->Play("繧｢繝ｼ繝槭メ繝･繧｢Action", true, false);
	m_gravity = 0;

	m_hp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;
	m_prevhp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;
	m_spawnTimer = 0;

	m_meshParticleRender->InitParticle();
	m_isBeingShot = false;

	ShockWave::Instance()->m_shockWave[moveID].m_radius = 0.0f;
	ShockWave::Instance()->m_shockWave[moveID].m_isActive = false;

	m_deadEffectData.m_dissolve.x = 0.0f;
	m_deadEffectData.m_dissolve.a = 0.0f;

	m_deadEffectData.m_outlineColor = OUTLINE_COLOR;
	m_deadEffectData.m_outlineColor.a = 0.0f;

	if (!m_deadParticle)
	{
		m_deadParticle = std::make_unique<EnemyDeadParticle>(m_meshParticleRender->meshParticleBufferData, m_transform.pos);
		m_deadParticle->m_vel = 2.0f;
	}
	if (!m_knockBackParticle)
	{
		m_knockBackParticle = std::make_unique<EnemyDeadParticle>(m_meshParticleRender->meshParticleBufferData, m_transform.pos);
		m_knockBackParticle->m_vel = 0.5f;
	}

	m_initDeadParticleFlag = false;


	m_shockWaveTimer = 1;
}

void VirusEnemy::Finalize()
{
}

void VirusEnemy::Update()
{
	using namespace KazMath;

	//HPを保存。
	m_prevhp = m_hp;
	m_hp = iEnemy_EnemyStatusData->oprationObjData->rockOnNum;

	//HPが減った瞬間だったらスケールを変える。
	if (m_hp != m_prevhp) {

		//攻撃を食らったときのリアクション用
		const float DEAD_EFFECT_SCALE = 1.0f;
		m_transform.scale += KazMath::Vec3<float>(DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE);

	}

	//敵が死んだ瞬間の処理を行う。
	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag && !m_isDead)
	{
		m_status = DEAD;

		m_isDead = true;
		iEnemy_EnemyStatusData->oprationObjData->initFlag = false;

		//攻撃を食らったときのリアクション用
		const float DEAD_EFFECT_SCALE = 1.0f;
		m_transform.scale += KazMath::Vec3<float>(DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE, DEAD_EFFECT_SCALE);

		ShakeMgr::Instance()->m_shakeAmount = 0.4f;
		SeaEffect::Instance()->ActivateSeaEffect();

		ShockWave::Instance()->m_shockWave[moveID].m_isActive = true;
		ShockWave::Instance()->m_shockWave[moveID].m_power = 1.0f;
		ShockWave::Instance()->m_shockWave[moveID].m_radius = 0.0f;

		m_shockWaveTimer = 0;

		//死亡エフェクトの初速度を計算。
		m_deadEffectVel *= DEAD_EFFECT_VEL;

		m_knockBackParticle->InitCompute(m_transform.pos, static_cast<UINT>(1000 * 3.5));
	}

	m_canLockOn = false;
	iEnemy_EnemyStatusData->curlNozieFlag = false;
	switch (m_status)
	{
	case VirusEnemy::APPEAR:
	{
		//現在の位置を確定。
		m_moveSineTimer += 0.06f;
		m_transform.pos.x = m_basePos.x;
		m_transform.pos.y = m_basePos.y + sinf(m_moveSineTimer) * 1.0f;

		//出現のタイマーを更新。
		m_appearEasingTimer = std::clamp(m_appearEasingTimer + 1.0f, 0.0f, APPEAR_EASING_TIMER * 10.0f);

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
		m_moveSineTimer += 0.06f;

		m_transform.pos.x = m_basePos.x;
		m_transform.pos.y = m_basePos.y + sinf(m_moveSineTimer) * 1.0f;

		m_transform.scale += (KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) - m_transform.scale) / 5.0f;

		m_canLockOn = true;
	}
	break;
	case VirusEnemy::DEAD:
	{
		m_transform.scale += (KazMath::Vec3<float>(VIRUS_SCALE, VIRUS_SCALE, VIRUS_SCALE) - m_transform.scale) / 5.0f;
		m_gravity += 0.002f;
		m_transform.pos.y -= m_gravity;
		m_transform.rotation.x += 2.0f;

		m_deadEffectData.m_dissolve.a += (1.0f - m_deadEffectData.m_dissolve.a) / 20.0f;

		m_deadEffectData.m_dissolve.x = std::clamp(m_deadEffectData.m_dissolve.x + 0.005f, 0.0f, 1.0f);

		m_deadEffectData.m_outlineColor.a += (1.0f - m_deadEffectData.m_outlineColor.a) / 50.0f;

		//死亡時のエフェクトを計算。
		m_transform.pos += m_deadEffectVel;
		m_deadEffectVel -= m_deadEffectVel / 6.0f;

		iEnemy_EnemyStatusData->curlNozieFlag = true;

		if (0.94f <= m_deadEffectData.m_dissolve.a && !m_initDeadParticleFlag)
		{
			m_deadParticle->InitCompute(m_transform.pos, 350);
			m_initDeadParticleFlag = true;

			SoundManager::Instance()->SoundPlayerWave(m_dispperSE, 0);
		}
		m_deadParticle->m_pos = m_transform.pos;

		//ノックバックのパーティクルを出させない
		if (0.6f <= m_deadEffectData.m_dissolve.a)
		{
			m_knockBackParticle->m_pos = { -10000.0f,0.0f,0.0f };
		}
		else
		{
			m_knockBackParticle->m_pos = m_transform.pos;
		}
	}
	break;
	default:
		break;
	}

	//衝撃波の計算
	m_shockWaveTimer = std::clamp(m_shockWaveTimer + 1.0f, 0.0f, SHOCK_WAVE_TIMER);
	//イージングを計算。
	float easing = EasingMaker(EasingType::Out, EaseInType::Cubic, m_shockWaveTimer / SHOCK_WAVE_TIMER);
	ShockWave::Instance()->m_shockWave[moveID].m_pos = m_basePos;
	ShockWave::Instance()->m_shockWave[moveID].m_radius = easing * SHOCK_WAVE_RAIDUS;
	ShockWave::Instance()->m_shockWave[moveID].m_power = (1.0f - easing);

	//衝撃波で動かす。
	m_basePos += m_shockWaveVel;
	if (0.01f < m_shockWaveVel.Length()) {
		m_shockWaveVel -= m_shockWaveVel / 20.0f;
	}
	else {
		m_basePos += (m_initPos - m_basePos) / 10.0f;
	}

	//プレイヤーの座標を保存。
	m_prevPlayerPos = m_playerTransform->pos;

	iEnemy_EnemyStatusData->hitBox.center = &m_transform.pos;
	iEnemy_EnemyStatusData->hitBox.radius = 10.0f;

	m_motherMat = m_transform.GetMat();

	m_animation->Update(1.0f - m_deadEffectData.m_dissolve.a);
	m_computeAnimation.Compute(
		*VertexBufferMgr::Instance()->GetVertexIndexBuffer(m_model.m_modelVertDataHandle).vertBuffer[0],
		m_animation->GetBoneMatBuff(),
		m_transform.GetMat()
	);


	if (iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		iEnemy_EnemyStatusData->curlNozieFlag = false;
	}
	else
	{
		iEnemy_EnemyStatusData->curlNozieFlag = true;
	}



	m_deadEffectData.m_outlineColor.a = 1;
	m_model.extraBufferArray[4].bufferWrapper->TransData(&m_deadEffectData, sizeof(m_deadEffectData));

	m_model.extraBufferArray.back() = GBufferMgr::Instance()->m_outlineBuffer;
	m_model.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
	m_model.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_TEX;

	ShockWave::Instance()->m_shockWave[moveID].m_facter = m_deadEffectData.m_dissolve.x;

	for (auto& obj : m_model.m_raytracingData.m_blas)
	{
		obj->Update();
	}
}

void VirusEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{

	DrawFunc::DrawModel(m_model, m_transform, m_animation->GetBoneMatBuff());
	arg_rasterize.ObjectRender(m_model);
	if (m_initDeadParticleFlag)
	{
		m_deadParticle->m_color = KazMath::Color(0, 200, 0, 255);
		m_deadParticle->UpdateCompute(arg_rasterize);

	}
	if (m_status == VirusEnemy::DEAD)
	{
		m_knockBackParticle->m_color = KazMath::Color(255, 0, 0, 255);
		m_knockBackParticle->UpdateCompute(arg_rasterize);
	}

	for (auto& index : m_model.m_raytracingData.m_blas)
	{
		arg_blasVec.Add(index, m_transform.GetMat(), moveID + 2);	//+2は0は通常オブジェクトで1はGPUパーティクルなため
	}


	std::vector<D3D12_RESOURCE_BARRIER> barrier;

	barrier.emplace_back(CD3DX12_RESOURCE_BARRIER::UAV(GBufferMgr::Instance()->m_outlineBuffer.bufferWrapper->GetBuffer().Get()));;

	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(static_cast<UINT>(barrier.size()), barrier.data());
}
