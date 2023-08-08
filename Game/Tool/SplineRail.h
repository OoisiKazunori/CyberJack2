#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Imgui/imgui.h"
#include"../Game/Debug/ParameterMgr.h"

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
	KazMath::Vec3<float> GetPosition();
private:
	//スプライン曲線の挙動---------------------------------------
	std::vector<KazMath::Vec3<float>>m_splineRailPosArray;
	int m_timer, m_maxTimer;
	int m_nowIndex;
	KazMath::Vec3<float>m_nowPosition;
	//スプライン曲線の挙動---------------------------------------

	//デバック用の処理---------------------------------------
	ParameterMgr m_jsonData;
	std::vector<DrawFuncData::DrawCallData> m_boxArray;
	std::vector<KazMath::Vec3<float>>m_posArray;
	int m_limitPosMaxNum;
	bool m_isLoopFlag;
	
	int m_vertexBufferHandle;//デバック用のスプライン曲線描画の頂点バッファ
	DrawFuncData::DrawCallData m_splineDrawCall;
	//デバック用の処理---------------------------------------

	void Import();
	void Export();
	void Reflect();
};

