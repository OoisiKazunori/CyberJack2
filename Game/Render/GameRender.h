#pragma once
#include"RenderTarget/RenderTargetStatus.h"
#include"PostEffect/GaussianBlur.h"
#include"DrawFunc/DrawFuncHelper.h"

/// <summary>
/// ポストエフェクト
/// </summary>
class GameRender
{
public:
	GameRender();
	void DrawPass(DrawingByRasterize& arg_rasterize);

private:
	std::unique_ptr<PostEffect::GaussianBlur> m_blurEmissive;

	DrawFuncHelper::TextureRender m_albedoAndNormalPass;//アルベドと法線の出力
	DrawFuncHelper::TextureRender m_emissivePass;		//エミッシブ
	DrawFuncHelper::TextureRender m_finalPass;			//最終合成

	RESOURCE_HANDLE m_compositeHandle;				//アルベド&法線とブルームの合成

	KazMath::Transform2D m_transform;
};

