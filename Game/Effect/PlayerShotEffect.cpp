#include "PlayerShotEffect.h"
#include <Render/DrawFunc.h>

PlayerShotEffect::PlayerShotEffect()
{

	m_vertexBufferHandle = -1;

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

}

void PlayerShotEffect::Update()
{

	//現在のフレームのタイマーを更新。
	++m_frame;

	//始点と終点の値を決める。
	std::array<KazMath::Vec3<float>, 4> controlPoints;
	controlPoints.front() = *m_refPlayerPos;
	controlPoints.back() = m_refEnemy->m_transform.pos;

	//制御点の場所を決める。
	controlPoints[1] = controlPoints[0] + m_controlPointVec;
	controlPoints[2] = controlPoints[3] + m_controlPointVec;

	//ベジエ曲線上の点を計算する。
	for (int index = 0; index < POINT_COUNT; ++index) {
		
		//このIndexの時間
		float time = static_cast<float>(index) / static_cast<float>(POINT_COUNT);

		m_splineRailPosArray[index] = EvaluateBezierCurve(controlPoints, time);

	}

	//現在の時間を01で求める。
	float nowTime = static_cast<float>(m_frame) / static_cast<float>(EFFECT_FRAME);
	//現在の時間以下のIndexの頂点をなくす。
	int vertexDeadline = std::clamp(static_cast<int>(nowTime * POINT_COUNT), 0, POINT_COUNT - 1);
	for (int index = 0; index < vertexDeadline; ++index) {
		m_splineRailPosArray[index] = m_splineRailPosArray[vertexDeadline];
	}

	//一定フレーム経過したら処理を終わらせる。
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