#include "SplineRail.h"

SplineRail::SplineRail()
{
	for (int i = 0; i < 1; ++i)
	{
		//m_boxArray[i] = DrawFuncData::SetDrawPolygonData();
	}
	//スプライン曲線の配置
	m_splineRailPosArray.emplace_back(KazMath::Vec3<float>(0.0f, 0.0f, 0.0f));
	m_splineRailPosArray.emplace_back(KazMath::Vec3<float>(0.0f, 0.0f, 0.0f));
	m_splineRailPosArray.emplace_back(KazMath::Vec3<float>(0.0f, 30.0f, 50.0f));
	m_splineRailPosArray.emplace_back(KazMath::Vec3<float>(0.0f, 0.0f, 100.0f));
	m_splineRailPosArray.emplace_back(KazMath::Vec3<float>(0.0f, 0.0f, 100.0f));

	Init();
}

void SplineRail::Init()
{
	m_timer = 0;
	m_maxTimer = KazMath::ConvertSecondToFlame(3);
	m_nowIndex = 1;
}

void SplineRail::Update()
{
	//制御点が0個ならゲーム進行不能のためアサートをはかせる
	if (m_splineRailPosArray.size() == 0)
	{
		assert(0);
	}

	++m_timer;
	if (m_maxTimer <= m_timer)
	{
		m_timer = 0;
		++m_nowIndex;

		if (IsEnd())
		{
			m_nowIndex = static_cast<int>(m_splineRailPosArray.size() - 2);
		}
	}
	m_nowPosition = KazMath::SplinePosition(m_splineRailPosArray, m_nowIndex, static_cast<float>(m_timer) / static_cast<float>(m_maxTimer), false);
}

void SplineRail::DebugDraw(DrawingByRasterize& arg_rasterize)
{
	ImGui::Begin("SplineLine");
	//ImGui::DragFloat("", );
	ImGui::End();

	//制御点の描画
	for (auto& render : m_boxArray)
	{
		//DrawFunc::DrawTextureIn2D(render, {});
		//arg_rasterize.ObjectRender(render);
	}
}

bool SplineRail::IsEnd()
{
	return (m_splineRailPosArray.size() - 2) <= m_nowIndex;
}

const KazMath::Vec3<float>& SplineRail::GetPosition()
{
	return m_nowPosition;
}
