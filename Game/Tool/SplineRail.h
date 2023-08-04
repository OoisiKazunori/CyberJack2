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
	/// �X�v���C���Ȑ��ɉ����������𓾂�
	/// </summary>
	const KazMath::Vec3<float>& GetPosition();
private:
	//�X�v���C���Ȑ��̋���---------------------------------------
	std::vector<KazMath::Vec3<float>>m_splineRailPosArray;
	int m_timer, m_maxTimer;
	int m_nowIndex;
	KazMath::Vec3<float>m_nowPosition;
	//�X�v���C���Ȑ��̋���---------------------------------------

	//�f�o�b�N�p�̏���---------------------------------------
	std::vector<DrawFuncData::DrawCallData> m_boxArray;
	//�f�o�b�N�p�̏���---------------------------------------
};

