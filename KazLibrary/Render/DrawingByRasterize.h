#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Math/KazMath.h"
#include"../Pipeline/Shader.h"
#include"../KazLibrary/Buffer/BufferDuplicateBlocking.h"
#include"../KazLibrary/Buffer/DrawFuncData.h"

/// <summary>
/// スタックされた情報を元にラスタライズで描画する
/// </summary>
class DrawingByRasterize
{
public:
	DrawingByRasterize();

	/// <summary>
	/// 描画情報にスタックした情報の初期化
	/// シーン切り替え時に呼び出し推奨
	/// </summary>
	void Clear();


	//描画情報のスタック----------------------------------------

	RESOURCE_HANDLE GetHandle();
	DrawFuncData::DrawData* StackData(RESOURCE_HANDLE HANDLE);
	//描画情報のスタック----------------------------------------

	//新システムでの描画命令
	void ObjectRender(const DrawFuncData::DrawCallData& DRAW_DATA);
	void Sort();
	void Render();

private:
	HandleMaker drawHandle;
	std::vector<DrawFuncData::DrawData> graphicDataArray;

	//パイプラインの情報----------------------------------------

	PipelineDuplicateBlocking piplineBufferMgr;
	ShaderDuplicateBlocking shaderBufferMgr;
	RootSignatureDuplicateBlocking rootSignatureBufferMgr;

	//パイプラインの情報----------------------------------------

	//レンダーターゲット情報----------------------------------------
	RESOURCE_HANDLE prevRenderTargetHandle;
	struct RenderTargetClearData
	{
		RESOURCE_HANDLE handle;
		bool isOpenFlag;
		bool clearFlag;
	};
	std::vector<RenderTargetClearData>renderTargetClearArray;
	//レンダーターゲット情報----------------------------------------


	//新しい描画命令の受け取り
	//描画情報
	std::vector<DrawFuncData::DrawData> renderInfomationForDirectX12Array;
	//描画命令の受け取り
	std::list<DrawFuncData::DrawCallData> kazCommandList;


	//描画に必要なバッファをコマンドリストに積む
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData>& BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);

	//頂点情報のセット
	void MultiMeshedDrawIndexInstanceCommand(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& DATA, const std::vector<std::vector<KazBufferHelper::BufferData>>& MATERIAL_BUFFER, std::vector<RootSignatureParameter> ROOT_PARAM);
	void DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData& DATA);
	void DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData& DATA);

	void DrawExecuteIndirect(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& DATA, const Microsoft::WRL::ComPtr<ID3D12CommandSignature>&arg_commandSignature,const DrawFuncData::ExcuteIndirectArgumentData &arg_argmentData);

	//何処の描画関数から呼び出されたかエラー文を書く
	void ErrorCheck(RESOURCE_HANDLE HANDLE, const std::source_location& DRAW_SOURCE_LOCATION)
	{
		if (HANDLE == -1)
		{
			FailCheck(ErrorMail(DRAW_SOURCE_LOCATION));
			assert(0);
		}
	}

	std::string ErrorMail(const std::source_location& DRAW_SOURCE_LOCATION);
};