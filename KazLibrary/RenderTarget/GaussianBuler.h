#pragma once
#include"../DirectXCommon/Base.h"
#include"../Buffer/CreateGpuBuffer.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Pipeline/GraphicsRootSignature.h"
#include"../Pipeline/GraphicsPipeLineMgr.h"
#include"../RenderTarget/RenderTargetStatus.h"
#include"../Helper/KazRenderHelper.h"
#include"../Render/KazRender.h"

/// <summary>
/// ガウシアンブラーがかかった画像を作成します
/// </summary>
class GaussianBuler
{
public:
	/// <summary>
	/// ガウシアンブラーがかった画像を作る前準備を行います
	/// </summary>
	/// <param name="GRAPH_SIZE">生成する画像サイズ</param>
	/// <param name="COLOR">画像の背景の色</param>
	GaussianBuler(const KazMath::Vec2<UINT> &GRAPH_SIZE);
	~GaussianBuler();

	/// <summary>
	///  レンダーターゲット内で描画されたオブジェクトをぼかします
	///　※レンダーターゲット内でこの関数を呼ぶ時はCURRENT_RENDERTARGETに現在のレンダーターゲットを入れてください
	/// </summary>
	/// <param name="RENDERTARGET_HANDLE">ぼかす対象が描画されているレンダーターゲット</param>
	/// <param name="CURRENT_RENDERTARGET">現在のレンダーターゲット</param>
	RESOURCE_HANDLE BlurImage(RESOURCE_HANDLE RENDERTARGET_HANDLE, RESOURCE_HANDLE CURRENT_RENDERTARGET_HANDLE = -1);

private:
	KazMath::Vec2<UINT> graphSize;

	//ガウシアンブラーした結果
	std::array<Sprite2DRender, 2> tex;
	KazRenderHelper::RenderInstancesData renderData;
	RESOURCE_HANDLE cbvHandle;

	CD3DX12_RECT rect;
	CD3DX12_VIEWPORT viewport;


	unique_ptr<CreateGpuBuffer> buffers;



	RESOURCE_HANDLE renderTargetBlurUpHandle;
	RESOURCE_HANDLE renderTargetBlurSideHandle;
};

