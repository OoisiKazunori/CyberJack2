#include "PlayerShotEffect.h"
#include <Render/DrawFunc.h>

PlayerShotEffect::PlayerShotEffect()
{


}

void PlayerShotEffect::Init()
{

	m_frame = 0;

}

void PlayerShotEffect::Generate(const KazMath::Vec3<float>* arg_refPlayerPos, shared_ptr<IEnemy> arg_refEnemy)
{
	//制御点を決めるベクトルを求める。
	m_controlPointVec = KazMath::Vec3<float>(KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R), KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R), KazMath::Rand(-CONTROL_POINT_R, CONTROL_POINT_R));

	m_frame = 0;
	m_refPlayerPos = arg_refPlayerPos;
	m_refEnemy = arg_refEnemy;
	m_isActive = true;
}

void PlayerShotEffect::Update()
{

	//Line描画用の処理
	m_splineRailPosArray.clear();
	m_splineRailPosArray.emplace_back(*m_refPlayerPos);
	m_splineRailPosArray.emplace_back(m_refEnemy->m_transform.pos);
	if (m_vertexBufferHandle != -1)
	{
		VertexBufferMgr::Instance()->ReleaseVeretexBuffer(m_vertexBufferHandle);
	}
	VertexGenerateData generateData(m_splineRailPosArray.data(), sizeof(DirectX::XMFLOAT3), m_splineRailPosArray.size(), sizeof(m_splineRailPosArray[0]));
	m_vertexBufferHandle = VertexBufferMgr::Instance()->GenerateBuffer(generateData, false);
	m_splineDrawCall = DrawFuncData::SetLine(m_vertexBufferHandle);

	//一定フレーム経過したら処理を終わらせる。
	++m_frame;
	if (EFFECT_FRAME <= m_frame) {
		m_refEnemy->Dead();
		m_isActive = false;
	}

}

void PlayerShotEffect::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{

	DrawFunc::DrawLine(m_splineDrawCall, m_splineRailPosArray, m_vertexBufferHandle);
	arg_rasterize.ObjectRender(m_splineDrawCall);

}
