#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

class SplineRail
{
public:
	SplineRail();

	void Init();
	void Update();
	void DebugDraw(DrawingByRasterize& arg_rasterize);

	bool IsEnd();
	/// <summary>
	/// スプライン曲線に沿った動きを得る
	/// </summary>
	const KazMath::Vec3<float>& GetPosition();
private:
	//スプライン曲線の挙動---------------------------------------
	std::vector<KazMath::Vec3<float>>m_splineRailPosArray;
	int m_timer, m_maxTimer;
	int m_nowIndex;
	KazMath::Vec3<float>m_nowPosition;
	//スプライン曲線の挙動---------------------------------------

	//デバック用の処理---------------------------------------
	std::vector<DrawFuncData::DrawCallData> m_boxArray;
	//デバック用の処理---------------------------------------
};

