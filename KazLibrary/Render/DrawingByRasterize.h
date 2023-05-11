#pragma once
#include<vector>
#include <iostream>
#include <source_location>
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
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

	void Update();
	void Draw();

	struct DrawData
	{
		bool generateFlag;

		//頂点情報
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;

		//パイプライン情報
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineData;
		std::vector<ShaderOptionData> shaderDataArray;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE pipelineHandle;
		std::vector<RESOURCE_HANDLE> shaderHandleArray;
		RESOURCE_HANDLE rootsignatureHandle;


		//その他描画に必要なバッファ情報
		std::vector<KazBufferHelper::BufferData> buffer;

		//デバック情報
		std::source_location drawCallData;

		DrawData() :generateFlag(false), renderTargetHandle(-1)
		{
		};
	};


	/// <summary>
	/// 描画情報にスタックした情報の初期化
	/// シーン切り替え時に呼び出し推奨
	/// </summary>
	void Clear();


	//描画情報のスタック----------------------------------------

	RESOURCE_HANDLE GetHandle();
	DrawData *StackData(RESOURCE_HANDLE HANDLE);

	//描画情報のスタック----------------------------------------

private:
	HandleMaker drawHandle;
	std::vector<DrawData> graphicDataArray;

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


	//描画に必要なバッファをコマンドリストに積む
	void SetBufferOnCmdList(const  std::vector<KazBufferHelper::BufferData> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM);

	//頂点情報のセット
	void DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData &DATA);
	void DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData &DATA);


	//何処の描画関数から呼び出されたかエラー文を書く
	void ErrorCheck(RESOURCE_HANDLE HANDLE, const std::source_location &DRAW_SOURCE_LOCATION)
	{
		if (HANDLE == -1)
		{
			FailCheck(ErrorMail(DRAW_SOURCE_LOCATION));
			assert(0);
		}
	}

	std::string ErrorMail(const std::source_location &DRAW_SOURCE_LOCATION);
};

namespace DrawFunc
{
	struct PipelineGenerateData
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
		std::vector<ShaderOptionData>shaderDataArray;
	};
	struct DrawFuncBufferData
	{
		DrawFuncBufferData(const KazBufferHelper::BufferResourceData &DATA, GraphicsRootParamType ROOT_PARAM, GraphicsRangeType RANGE_TYPE) :
			resourceData(DATA), rootParam(ROOT_PARAM), rangeType(RANGE_TYPE)
		{};

		KazBufferHelper::BufferResourceData resourceData;
		GraphicsRootParamType rootParam;
		GraphicsRangeType rangeType;
	};


	//DrawFuncを使用する際に必要なデータ
	struct DrawCallData
	{
		DrawCallData(DrawingByRasterize *CALL_DATA_PTR) :
			callDataPtr(CALL_DATA_PTR)
		{
		};
		//ラスタライザ描画呼び出し
		DrawingByRasterize *callDataPtr;

		//頂点情報
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;

		//パイプライン情報
		PipelineGenerateData pipelineData;

		//その他必要なバッファの設定
		std::vector<DrawFuncBufferData>bufferResourceDataArray;
	};


	//単色のポリゴン表示(インデックスあり)
	static DrawCallData SetDrawPolygonIndexData(DrawingByRasterize *CALL_DATA_PTR, const KazRenderHelper::DrawIndexInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData(CALL_DATA_PTR);
		//頂点情報
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		//行列情報
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX)),
			GRAPHICS_PRAMTYPE_DATA,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		//色情報
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4)),
			GRAPHICS_PRAMTYPE_DATA2,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);		//パイプライン情報のセット
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//行列情報のみ
	static DrawCallData SetTransformData(DrawingByRasterize *CALL_DATA_PTR, const KazRenderHelper::DrawIndexInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData(CALL_DATA_PTR);
		//頂点情報
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		//行列情報
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX)),
			GRAPHICS_PRAMTYPE_DATA,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};


	//単色のポリゴン表示(インデックスなし)
	static DrawCallData SetDrawPolygonData(DrawingByRasterize *CALL_DATA_PTR, const KazRenderHelper::DrawInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData(CALL_DATA_PTR);
		//頂点情報
		lDrawCallData.drawInstanceCommandData = VERTEX_DATA;
		//行列情報
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX)),
			GRAPHICS_PRAMTYPE_DATA,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		//色情報
		lDrawCallData.bufferResourceDataArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4)),
			GRAPHICS_PRAMTYPE_DATA2,
			GRAPHICS_RANGE_TYPE_CBV_VIEW
		);
		//パイプライン情報のセット
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};


	//クラスを使用した描画関数
	class KazRender
	{
	public:
		KazRender(const DrawCallData &INIT_DATA, std::source_location location = std::source_location::current()) :
			callData(INIT_DATA.callDataPtr),
			handle(callData->GetHandle())
		{
			//バッファの生成
			DrawingByRasterize::DrawData *lData = callData->StackData(handle);

			//頂点情報のセット
			lData->drawInstanceCommandData = INIT_DATA.drawInstanceCommandData;
			lData->drawIndexInstanceCommandData = INIT_DATA.drawIndexInstanceCommandData;

			//パイプラインの設定
			lData->pipelineData = INIT_DATA.pipelineData.desc;
			lData->shaderDataArray = INIT_DATA.pipelineData.shaderDataArray;

			//その他必要なバッファのセット
			for (int i = 0; i < INIT_DATA.bufferResourceDataArray.size(); ++i)
			{
				lData->buffer.emplace_back(INIT_DATA.bufferResourceDataArray[i].resourceData);
				lData->buffer[i].rangeType = INIT_DATA.bufferResourceDataArray[i].rangeType;
				lData->buffer[i].rootParamType = INIT_DATA.bufferResourceDataArray[i].rootParam;
			}

			//デバック用の情報のセット
			lData->drawCallData = location;
		};


		//よく使う処理は関数に纏める----------------------------------------

		//用意したバッファのアクセス
		DrawingByRasterize::DrawData *GetDrawData()
		{
			return callData->StackData(handle);
		}

		void DrawCall(const KazMath::Transform3D &TRANSFORM, const KazMath::Color &COLOR, int CAMERA_INDEX, const DirectX::XMMATRIX &MOTHER_MAT)
		{
			//スタック用のバッファを呼び出し、そこに入っているバッファを使用して転送する
			DrawingByRasterize::DrawData *lData = GetDrawData();
			//行列
			DirectX::XMMATRIX lMat =
				KazMath::CaluWorld(TRANSFORM, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f }) *
				CameraMgr::Instance()->GetViewMatrix() *
				CameraMgr::Instance()->GetPerspectiveMatProjection();
			lData->buffer[0].bufferWrapper->TransData(&lMat, sizeof(DirectX::XMMATRIX));
			//色
			DirectX::XMFLOAT4 lColor = COLOR.ConvertColorRateToXMFLOAT4();
			lData->buffer[1].bufferWrapper->TransData(&lColor, sizeof(DirectX::XMFLOAT4));
		};

		void DrawTexPlane(const KazMath::Transform3D &TRANSFORM, const KazMath::Color &COLOR, int CAMERA_INDEX, const DirectX::XMMATRIX &MOTHER_MAT)
		{
			//スタック用のバッファを呼び出し、そこに入っているバッファを使用して転送する
			DrawingByRasterize::DrawData *lData = GetDrawData();
			//行列
			DirectX::XMMATRIX lMat =
				KazMath::CaluWorld(TRANSFORM, { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f }) *
				//CameraMgr::Instance()->GetViewMatrix() *
				CameraMgr::Instance()->GetOrthographicMatProjection();
			lData->buffer[0].bufferWrapper->TransData(&lMat, sizeof(DirectX::XMMATRIX));
		};

		//よく使う処理は関数に纏める----------------------------------------

	private:
		DrawingByRasterize *callData;
		RESOURCE_HANDLE handle;
	};
}