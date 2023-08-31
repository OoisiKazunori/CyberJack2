#pragma once
#include"../DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"../KazLibrary/RenderTarget/GaussianBuler.h"
#include"../KazLibrary/Helper/Select.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Player.h"
#include"../Game/GameRenderTarget.h"

class TitleScene :public SceneBase
{
public:
	TitleScene();

	void Init();
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

private:
	DrawFunc::TextureRender m_clearRender;
	DrawFunc::TextureRender m_inputRender;

	int m_sceneNum;
	bool m_flashFlag;
	int m_flashTimer;
};