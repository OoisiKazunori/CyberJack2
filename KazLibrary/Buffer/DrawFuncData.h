#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Buffer/VertexBufferMgr.h"
#include <source_location>
#include"GBufferMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

namespace Raytracing {
	class Blas;
}

namespace DrawFuncPipelineData
{
	enum class PipelineBlendModeEnum
	{
		NONE = -1,
		ALPHA,
		ADD,
		SUB,
		MAX
	};

	//α
	static D3D12_RENDER_TARGET_BLEND_DESC SetAlphaBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC lAlphaBlendDesc{};
		lAlphaBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		lAlphaBlendDesc.BlendEnable = true;
		lAlphaBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		lAlphaBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		lAlphaBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		lAlphaBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		lAlphaBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		lAlphaBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		return lAlphaBlendDesc;
	};

	//加算合成
	static D3D12_RENDER_TARGET_BLEND_DESC SetAddBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC lAddBlendDesc{};
		lAddBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		lAddBlendDesc.BlendEnable = true;
		lAddBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		lAddBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		lAddBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		lAddBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		lAddBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		lAddBlendDesc.DestBlend = D3D12_BLEND_ONE;
		return lAddBlendDesc;
	}

	//減算合成
	static D3D12_RENDER_TARGET_BLEND_DESC SetSubBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC subBlendDesc{};
		subBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		subBlendDesc.BlendEnable = true;
		subBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		subBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		subBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		subBlendDesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
		subBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		subBlendDesc.DestBlend = D3D12_BLEND_ONE;

		return subBlendDesc;
	}

	//反転合成
	static D3D12_RENDER_TARGET_BLEND_DESC SetRevBlend()
	{
		D3D12_RENDER_TARGET_BLEND_DESC lRevBlendDesc{};
		lRevBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;;
		lRevBlendDesc.BlendEnable = true;
		lRevBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		lRevBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		lRevBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		lRevBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		lRevBlendDesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
		lRevBlendDesc.DestBlend = D3D12_BLEND_ZERO;

		return lRevBlendDesc;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetTest()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};



		static D3D12_INPUT_ELEMENT_DESC input3DLayOut =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		gPipeline.InputLayout.pInputElementDescs = &input3DLayOut;
		gPipeline.InputLayout.NumElements = 1;

		//サンプルマスク
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//ラスタライザ
		//背面カリング、塗りつぶし、深度クリッピング有効
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		gPipeline.RasterizerState = rasterrize;
		//ブレンドモード
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//図形の形状
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//その他設定
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//デプスステンシルステートの設定
		gPipeline.DepthStencilState.DepthEnable = true;							//深度テストを行う
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//小さければOK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//深度値フォーマット

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetTex()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[2];
		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 2;

		//サンプルマスク
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//ラスタライザ
		//背面カリング、塗りつぶし、深度クリッピング有効
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;

		//ブレンドモード
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//図形の形状
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//その他設定
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//デプスステンシルステートの設定
		gPipeline.DepthStencilState.DepthEnable = true;							//深度テストを行う
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//小さければOK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//深度値フォーマット

		return gPipeline;
	}



	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosNormal()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[2];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 2;

		//サンプルマスク
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//ラスタライザ
		//背面カリング、塗りつぶし、深度クリッピング有効
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		gPipeline.RasterizerState = rasterrize;
		//ブレンドモード
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//図形の形状
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//その他設定
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//デプスステンシルステートの設定
		gPipeline.DepthStencilState.DepthEnable = true;							//深度テストを行う
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//小さければOK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//深度値フォーマット

		return gPipeline;
	}

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormal()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[3];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[2] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 3;

		//サンプルマスク
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//ラスタライザ
		//背面カリング、塗りつぶし、深度クリッピング有効
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//ブレンドモード
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//図形の形状
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//その他設定
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//デプスステンシルステートの設定
		gPipeline.DepthStencilState.DepthEnable = true;							//深度テストを行う
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//小さければOK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//深度値フォーマット

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormalTangentBinormal()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[5];

		input3DLayOut[0] =
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[1] =
		{
			"NORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[2] =
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[3] =
		{
			"TANGENT",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[4] =
		{
			"BINORMAL",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 5;

		//サンプルマスク
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//ラスタライザ
		//背面カリング、塗りつぶし、深度クリッピング有効
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//ブレンドモード
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//図形の形状
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//その他設定
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//デプスステンシルステートの設定
		gPipeline.DepthStencilState.DepthEnable = true;							//深度テストを行う
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//書き込み許可
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//小さければOK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//深度値フォーマット

		return gPipeline;
	}
};

namespace DrawFuncData
{
	class DrawingByRasterize;
	enum class VERT_TYPE
	{
		INDEX,
		INSTANCE,
		MULTI_MESHED
	};

	struct DrawData
	{
		bool generateFlag;

		//頂点情報
		KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData drawMultiMeshesIndexInstanceCommandData;
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;
		DrawFuncData::VERT_TYPE drawCommandType;
		//マテリアル情報
		std::vector<std::vector<KazBufferHelper::BufferData>> materialBuffer;


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

	struct PipelineGenerateData
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
		std::vector<ShaderOptionData>shaderDataArray;
		DrawFuncPipelineData::PipelineBlendModeEnum blendMode;
	};
	struct DrawFuncBufferData
	{
		DrawFuncBufferData(const KazBufferHelper::BufferResourceData& DATA, GraphicsRootParamType ROOT_PARAM, GraphicsRangeType RANGE_TYPE) :
			resourceData(DATA), rootParam(ROOT_PARAM), rangeType(RANGE_TYPE)
		{};

		KazBufferHelper::BufferResourceData resourceData;
		GraphicsRootParamType rootParam;
		GraphicsRangeType rangeType;
	};

	//DrawCallData内で使用するレイトレーシング関連の情報
	struct RaytracingData {
		std::vector<std::shared_ptr<Raytracing::Blas>> m_blas;	//レイトレーシングに使用するBlas。レイトレーシングを行う場合、これをBlasVectorに参照を保存する。
		bool m_isRaytracingInitialized;							//レイトレーシングに必要な情報が初期化されているか。
		bool m_isRaytracingEnable;								//レイトレーシングが有効化されているか。 m_isRaytracingInitialized(初期化されている) and m_isRaytracingEnable(有効化されている) 時にTLASに保存される。 ラスタライズの描画は行うけどレイトレは一旦切る場合に使用することを考慮して作成しました。距離によってカリングしたりする場合ですね！
	};

	//DrawFuncを使用する際に必要なデータ
	struct DrawCallData
	{
		DrawCallData(std::source_location location = std::source_location::current()) :
			callLocation(location), renderTargetHandle(-1)
		{
		};
		/// <summary>
		/// レイトレーシングを準備
		/// </summary>
		/// <param name="arg_isOpaque"> このオブジェクトは不透明か？AnyhitShaderが呼ばれるかどうかをここで判断する。 </param>
		void SetupRaytracing(bool arg_isOpaque);
		//頂点情報
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;
		KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData drawMultiMeshesIndexInstanceCommandData;
		VERT_TYPE drawCommandType;
		std::vector<std::vector<KazBufferHelper::BufferData>> materialBuffer;

		//レイトレーシングに使用する情報
		RaytracingData m_raytracingData;

		//頂点情報が格納されているデータのハンドル
		RESOURCE_HANDLE m_modelVertDataHandle;

		RESOURCE_HANDLE renderTargetHandle;
		//パイプライン情報
		PipelineGenerateData pipelineData;

		//その他必要なバッファの設定
		std::vector<KazBufferHelper::BufferData>extraBufferArray;

		std::source_location callLocation;
	};

	//単色のポリゴン表示(インデックスなし)
	static DrawCallData SetDrawPolygonData(const KazRenderHelper::DrawInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//頂点情報
		lDrawCallData.drawInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::INSTANCE;

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//色情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//パイプライン情報のセット
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//単色のポリゴン表示(インデックスあり)
	static DrawCallData SetDrawPolygonIndexData(const KazRenderHelper::DrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA, std::source_location location = std::source_location::current())
	{
		DrawCallData lDrawCallData;
		//頂点情報
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::INDEX;

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//色情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//パイプライン情報のセット
		lDrawCallData.pipelineData = PIPELINE_DATA;

		lDrawCallData.callLocation = location;

		return lDrawCallData;
	};

	//OBJモデルのポリゴン表示(インデックスあり)
	static DrawCallData SetDrawOBJIndexData(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//頂点情報
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//マテリアル情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(MaterialBufferData))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//パイプライン情報のセット
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//モデルのポリゴン表示(インデックスあり、マテリアルあり)
	static DrawCallData SetDrawGLTFIndexMaterialData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;

		lDrawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//頂点情報
		lDrawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetBuffer(MODEL_DATA.modelVertDataHandle).index;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			lDrawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		lDrawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		lDrawCallData.pipelineData = PIPELINE_DATA;
		lDrawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return lDrawCallData;
	};

	//モデルのポリゴン表示(インデックスあり、マテリアルあり)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingData(const ModelInfomation &MODEL_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;

		lDrawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//頂点情報
		lDrawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetBuffer(MODEL_DATA.modelVertDataHandle).index;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto &obj : MODEL_DATA.modelData)
		{
			lDrawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		lDrawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//レイトレ側での判断
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		lDrawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		lDrawCallData.pipelineData = PIPELINE_DATA;
		lDrawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return lDrawCallData;
	};


	//行列情報のみ
	static DrawCallData SetTransformData(const KazRenderHelper::DrawIndexInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//頂点情報
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::INDEX;

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//行列情報とテクスチャ
	static DrawCallData SetTexPlaneData(const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
		//頂点情報
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetBuffer(handle).index;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//行列情報
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//テクスチャ情報
		lDrawCallData.extraBufferArray.emplace_back();
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};


	static DrawCallData SetLineData(const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//std::vector<VertexGenerateData>vertexMesh;
		//vertexMesh.emplace_back();
		//RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GenerateBuffer();
		//SetDrawPolygonData(VertexBufferMgr::Instance()->GetBuffer(handle).index, PIPELINE_DATA);
		return lDrawCallData;
	};


	static DrawCallData SetDefferdRenderingModel(std::shared_ptr<ModelInfomation>arg_model)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//その他設定
		lData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			lData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}

		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdMain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdMain", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(*arg_model, lData);
		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		drawCall.SetupRaytracing(true);

		return drawCall;
	};

}