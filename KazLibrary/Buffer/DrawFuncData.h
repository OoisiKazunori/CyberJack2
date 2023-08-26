#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Loader/ModelLoader.h"
#include"../KazLibrary/Buffer/VertexBufferMgr.h"
#include <source_location>
#include"GBufferMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

namespace Raytracing
{
	class Blas;
}

namespace ExecuteIndirectData
{
	struct DrawIndexedIndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS m_view;
		D3D12_DRAW_INDEXED_ARGUMENTS m_drawArguments;
	};
	struct DrawIndirectCommand
	{
		D3D12_GPU_VIRTUAL_ADDRESS m_view;
		D3D12_DRAW_ARGUMENTS m_drawArguments;
	};

	static void GenerateCommandSignature(
		Microsoft::WRL::ComPtr<ID3D12CommandSignature>& arg_commandSignature,
		const Microsoft::WRL::ComPtr<ID3D12RootSignature>& arg_rootSignature,
		const std::vector<D3D12_INDIRECT_ARGUMENT_DESC>& arg_indirectDescArray)
	{
		//コマンドシグネチャ---------------------------
		D3D12_COMMAND_SIGNATURE_DESC desc{};
		desc.pArgumentDescs = arg_indirectDescArray.data();
		desc.NumArgumentDescs = static_cast<UINT>(arg_indirectDescArray.size());
		desc.ByteStride = sizeof(DrawIndexedIndirectCommand);

		HRESULT lResult =
			DirectX12Device::Instance()->dev->CreateCommandSignature(&desc, arg_rootSignature.Get(), IID_PPV_ARGS(&arg_commandSignature));
		//コマンドシグネチャ---------------------------
		if (lResult != S_OK)
		{
			assert(0);
		}
	}
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

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPos()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut;
		input3DLayOut =
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
		//塗りつぶし、深度クリッピング有効
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


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormalTangentBinormalBoneNoWeight()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut[7];

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
		input3DLayOut[5] =
		{
			"BONE_NO",
			0,
			DXGI_FORMAT_R32G32B32A32_SINT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		input3DLayOut[6] =
		{
			"WEIGHT",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};

		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
		gPipeline.InputLayout.NumElements = 7;

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

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosLine()
	{
		//パイプラインの設定
		D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};

		static D3D12_INPUT_ELEMENT_DESC input3DLayOut;
		input3DLayOut =
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
		//塗りつぶし、深度クリッピング有効
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		rasterrize.FillMode = D3D12_FILL_MODE_WIREFRAME;
		gPipeline.RasterizerState = rasterrize;
		//ブレンドモード
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//図形の形状
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

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
		MULTI_MESHED,
		EXECUTEINDIRECT_INDEX,
		EXECUTEINDIRECT_INSTANCE,
	};

	struct ExcuteIndirectArgumentData
	{
		KazBufferHelper::BufferData m_uploadArgumentBuffer;
		KazBufferHelper::BufferData m_uavArgumentBuffer;
		UINT m_maxCommandCount;
		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> m_desc;
		ExecuteIndirectData::DrawIndexedIndirectCommand m_argumentCommandData;

		void GenerateArgumentBuffer();
	};

	//描画命令の発行を行うデータ
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
		//ExcuteIndirect発行命令
		ExcuteIndirectArgumentData m_executeIndirectGenerateData;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature>m_commandSignature;
		RESOURCE_HANDLE m_commandRootsignatureHandle;

		//パイプライン情報
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineData;
		std::vector<ShaderOptionData> shaderDataArray;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE depthHandle;
		RESOURCE_HANDLE pipelineHandle;
		std::vector<RESOURCE_HANDLE> shaderHandleArray;
		RESOURCE_HANDLE m_rootsignatureHandle;


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
			callLocation(location), renderTargetHandle(-1), depthHandle(-1)
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

		//ExecuteIndirect
		ExcuteIndirectArgumentData m_executeIndirectGenerateData;

		//レイトレーシングに使用する情報
		RaytracingData m_raytracingData;

		//頂点情報が格納されているデータのハンドル
		RESOURCE_HANDLE m_modelVertDataHandle;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE depthHandle;
		//パイプライン情報
		PipelineGenerateData pipelineData;

		//その他必要なバッファの設定
		std::vector<KazBufferHelper::BufferData>extraBufferArray;

		std::source_location callLocation;
	};

	//単色のポリゴン表示(インデックスなし)
	static DrawCallData SetDrawPolygonData(const KazRenderHelper::DrawInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;
		//頂点情報
		drawCallData.drawInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::INSTANCE;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//色情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//パイプライン情報のセット
		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//単色のポリゴン表示(インデックスあり)
	static DrawCallData SetDrawPolygonIndexData(const KazRenderHelper::DrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA, std::source_location location = std::source_location::current())
	{
		DrawCallData drawCallData;
		//頂点情報
		drawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::INDEX;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//色情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//パイプライン情報のセット
		drawCallData.pipelineData = PIPELINE_DATA;

		drawCallData.callLocation = location;

		return drawCallData;
	};

	//OBJモデルのポリゴン表示(インデックスあり)
	static DrawCallData SetDrawOBJIndexData(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;
		//頂点情報
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//マテリアル情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(MaterialBufferData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//パイプライン情報のセット
		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//モデルのポリゴン表示(インデックスあり、マテリアルあり)
	static DrawCallData SetDrawGLTFIndexMaterialData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//頂点情報
		drawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			drawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//色乗算
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//モデルのポリゴン表示(インデックスあり、マテリアルあり)
	static DrawCallData SetDrawGLTFIndexMaterialLightData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//頂点情報
		drawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			drawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//ライト情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT3))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT3);

		//色乗算
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//レイトレでのモデルのポリゴン表示(インデックスあり、マテリアルあり)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//頂点情報
		drawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			drawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//レイトレ側での判断
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//色乗算
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));



		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		return drawCallData;
	};

	//レイトレでのモデルのポリゴン表示(インデックスあり、マテリアルあり、ブルームの加減設定あり)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingBloomData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		drawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//頂点情報
		drawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			drawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//レイトレ側での判断
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//色乗算
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		//エミッシブ..xyz色,a強さ
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		drawCallData.SetupRaytracing(true);

		return drawCallData;
	};

	//レイトレでのモデルのポリゴン表示(インデックスあり、マテリアルあり、ブルームの加減設定あり、アニメーション対応)
	static DrawCallData SetDrawGLTFAnimationIndexMaterialInRayTracingBloomData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		//頂点情報
		drawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			drawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//レイトレ側での判断
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(UINT))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(UINT);

		//色乗算
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);
		KazMath::Color init(255, 255, 255, 255);
		drawCallData.extraBufferArray.back().bufferWrapper->TransData(&init.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));

		//エミッシブ..xyz色,a強さ
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = PIPELINE_DATA;
		drawCallData.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		drawCallData.SetupRaytracing(true);

		return drawCallData;
	};

	//レイトレのみセットアップ
	DrawCallData SetRaytracingData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA);


	//行列情報のみ
	static DrawCallData SetTransformData(const KazRenderHelper::DrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;
		//頂点情報
		drawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		drawCallData.drawCommandType = VERT_TYPE::INDEX;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};

	//行列情報とテクスチャ
	static DrawCallData SetTexPlaneData(const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
		//頂点情報
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(handle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//テクスチャ情報
		drawCallData.extraBufferArray.emplace_back();
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
	};


	static DrawCallData SetSpriteAlphaData(const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData drawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
		//頂点情報
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(handle).index;
		drawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//乗算
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//テクスチャ情報
		drawCallData.extraBufferArray.emplace_back();
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		drawCallData.pipelineData = PIPELINE_DATA;

		return drawCallData;
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

	static DrawCallData SetDefferdRenderingModelAnimation(std::shared_ptr<ModelInfomation>arg_model)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData lData;
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdAnimationMain", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(*arg_model, lData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.SetupRaytracing(true);

		return drawCall;
	};

	static DrawCallData SetDefferdRenderingModelAnimationDissolve(std::shared_ptr<ModelInfomation>arg_model)
	{
		DrawCallData drawCall;

		DrawFuncData::PipelineGenerateData lData;
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdAnimationMain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "PostEffect/Dissolve/" + "DissolveModel.hlsl", "PSDefferdAnimationMainDissolve", "ps_6_4", SHADER_TYPE_PIXEL);

		drawCall = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(*arg_model, lData);
		drawCall.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();

		drawCall.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			drawCall.pipelineData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		drawCall.pipelineData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());

		drawCall.extraBufferArray.emplace_back();
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA4;

		drawCall.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCall.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCall.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA5;

		drawCall.SetupRaytracing(true);

		return drawCall;
	};

	static DrawCallData SetLine(RESOURCE_HANDLE arg_vertexHandle)
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosLine();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawLine.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawLine.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		DrawCallData drawCallData;
		//頂点情報
		drawCallData.drawInstanceCommandData = VertexBufferMgr::Instance()->GetVertexBuffer(arg_vertexHandle).instanceData;
		drawCallData.drawCommandType = VERT_TYPE::INSTANCE;
		drawCallData.m_modelVertDataHandle = arg_vertexHandle;

		//行列情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMMATRIX);
		//色情報
		drawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		drawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		drawCallData.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT4);

		drawCallData.pipelineData = lData;
		drawCallData.drawInstanceCommandData.topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

		return drawCallData;
	}

	static DrawCallData SetExecuteIndirect(const PipelineGenerateData& PIPELINE_DATA, const D3D12_GPU_VIRTUAL_ADDRESS& arg_address, UINT arg_maxCountNum, UINT arg_indexNum = -1)
	{
		DrawCallData drawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
		//頂点情報
		drawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(handle).index;
		drawCallData.drawCommandType = VERT_TYPE::EXECUTEINDIRECT_INDEX;

		drawCallData.pipelineData = PIPELINE_DATA;

		//ExecuteIndirect発行
		drawCallData.m_executeIndirectGenerateData.m_maxCommandCount = arg_maxCountNum;

		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> args;
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
		args[0].UnorderedAccessView.RootParameterIndex = 0;
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;
		drawCallData.m_executeIndirectGenerateData.m_desc = args;

		drawCallData.m_executeIndirectGenerateData.m_argumentCommandData.m_view = arg_address;
		drawCallData.m_executeIndirectGenerateData.m_argumentCommandData.m_drawArguments =
		{
			drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].indexCountPerInstance,
			arg_maxCountNum,
			drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].startIndexLocation,
			(int)drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].baseVertexLocation,
			drawCallData.drawMultiMeshesIndexInstanceCommandData.drawIndexInstancedData[0].startInstanceLocation
		};

		if (arg_indexNum != -1)
		{
			drawCallData.m_executeIndirectGenerateData.m_argumentCommandData.m_drawArguments.IndexCountPerInstance = arg_indexNum;
		}

		drawCallData.m_executeIndirectGenerateData.m_uploadArgumentBuffer = KazBufferHelper::SetUploadBufferData(sizeof(ExecuteIndirectData::DrawIndexedIndirectCommand));
		drawCallData.m_executeIndirectGenerateData.m_uploadArgumentBuffer.bufferWrapper->TransData(&drawCallData.m_executeIndirectGenerateData.m_argumentCommandData, sizeof(ExecuteIndirectData::DrawIndexedIndirectCommand));

		drawCallData.m_executeIndirectGenerateData.m_uavArgumentBuffer = KazBufferHelper::SetGPUBufferData(sizeof(ExecuteIndirectData::DrawIndexedIndirectCommand));

		drawCallData.m_executeIndirectGenerateData.m_uavArgumentBuffer.bufferWrapper->CopyBuffer(
			drawCallData.m_executeIndirectGenerateData.m_uploadArgumentBuffer.bufferWrapper->GetBuffer());

		drawCallData.m_executeIndirectGenerateData.m_uavArgumentBuffer.bufferWrapper->ChangeBarrier(
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS
		);

		drawCallData.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		return drawCallData;
	};

	static DrawCallData SetParticleInRaytracing(const std::shared_ptr<KazBufferHelper::BufferData>& arg_buffer, const std::shared_ptr<KazBufferHelper::BufferData>& arg_indexBuffer)
	{
		DrawCallData drawCallData;
		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
		drawCallData.m_modelVertDataHandle = VertexBufferMgr::Instance()->StackVertexBuffer(arg_buffer, arg_indexBuffer);
		drawCallData.materialBuffer.emplace_back();
		drawCallData.materialBuffer.back().emplace_back(TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::TestPath + "white1x1.png"));
		drawCallData.materialBuffer.back().back().rootParamType = GRAPHICS_PRAMTYPE_TEX;
		drawCallData.SetupRaytracing(true);

		return drawCallData;
	};


	static DrawFuncData::PipelineGenerateData GetModelShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetModelBloomShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdMain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdBloomMain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//その他設定
		lData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			lData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetAnimationModelBloomShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormalBoneNoWeight();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSDefferdMain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSDefferdBloomMain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//その他設定
		lData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			lData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetSpriteShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetSpriteAlphaShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSAlphaMain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicInstanceShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//その他設定
		lData.desc.NumRenderTargets = static_cast<UINT>(GBufferMgr::Instance()->GetRenderTargetFormat().size());
		for (int i = 0; i < GBufferMgr::Instance()->GetRenderTargetFormat().size(); ++i)
		{
			lData.desc.RTVFormats[i] = GBufferMgr::Instance()->GetRenderTargetFormat()[i];
		}
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicInstanceShader2()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "BasicInstance.hlsl", "PS", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		//その他設定
		lData.desc.NumRenderTargets = 1;
		return lData;
	};

	static DrawFuncData::PipelineGenerateData GetBasicShader()
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPos();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Basic.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Basic.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
		//その他設定
		lData.desc.NumRenderTargets = 1;
		lData.desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		lData.desc.SampleDesc.Count = 1;
		return lData;
	};
}