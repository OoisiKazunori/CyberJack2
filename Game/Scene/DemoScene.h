#pragma once
#include"../Game/Scene/SceneBase.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/DrawFunc/DrawFuncHelper.h"
#include"../Game/Debug/DebugCamera.h"

class DemoScene :public SceneBase
{
public:
	DemoScene();
	~DemoScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

private:
	//�J����--------------------------
	DebugCamera m_camera;

	//�`��--------------------------
	DrawFuncHelper::TextureRender m_2DSprite, m_3DSprite;
	KazMath::Transform3D m_3DSpriteTransform;
	KazMath::Transform2D m_2DSpriteTransform;

	DrawFuncHelper::ModelRender m_modelAnimationRender,m_modelRender;
	KazMath::Transform3D m_modelAnimationTransform,m_modelTransform;


	//��--------------------------
	SoundData m_bgmHandle,m_seHandle;

	//�O���b�h�`��--------------------------
	std::array<DrawFuncData::DrawCallData, 11> m_gridCallDataX;
	std::array<DrawFuncData::DrawCallData, 11> m_gridCallDataZ;
};

