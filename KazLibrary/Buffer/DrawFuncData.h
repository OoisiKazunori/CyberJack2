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

	//��
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

	//���Z����
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

	//���Z����
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

	//���]����
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
		//�p�C�v���C���̐ݒ�
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

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetTex()
	{
		//�p�C�v���C���̐ݒ�
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

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;

		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}



	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosNormal()
	{
		//�p�C�v���C���̐ݒ�
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

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}

	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormal()
	{
		//�p�C�v���C���̐ݒ�
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

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

		return gPipeline;
	}


	static D3D12_GRAPHICS_PIPELINE_STATE_DESC SetPosUvNormalTangentBinormal()
	{
		//�p�C�v���C���̐ݒ�
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

		//�T���v���}�X�N
		gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		//���X�^���C�U
		//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
		CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
		rasterrize.CullMode = D3D12_CULL_MODE_NONE;
		gPipeline.RasterizerState = rasterrize;
		//�u�����h���[�h
		gPipeline.BlendState.RenderTarget[0] = SetAlphaBlend();

		//�}�`�̌`��
		gPipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		//���̑��ݒ�
		gPipeline.NumRenderTargets = 1;
		gPipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gPipeline.SampleDesc.Count = 1;

		//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
		gPipeline.DepthStencilState.DepthEnable = true;							//�[�x�e�X�g���s��
		gPipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
		gPipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;		//���������OK
		gPipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;							//�[�x�l�t�H�[�}�b�g

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

		//���_���
		KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData drawMultiMeshesIndexInstanceCommandData;
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;
		DrawFuncData::VERT_TYPE drawCommandType;
		//�}�e���A�����
		std::vector<std::vector<KazBufferHelper::BufferData>> materialBuffer;


		//�p�C�v���C�����
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineData;
		std::vector<ShaderOptionData> shaderDataArray;

		RESOURCE_HANDLE renderTargetHandle;
		RESOURCE_HANDLE pipelineHandle;
		std::vector<RESOURCE_HANDLE> shaderHandleArray;
		RESOURCE_HANDLE rootsignatureHandle;


		//���̑��`��ɕK�v�ȃo�b�t�@���
		std::vector<KazBufferHelper::BufferData> buffer;

		//�f�o�b�N���
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

	//DrawCallData���Ŏg�p���郌�C�g���[�V���O�֘A�̏��
	struct RaytracingData {
		std::vector<std::shared_ptr<Raytracing::Blas>> m_blas;	//���C�g���[�V���O�Ɏg�p����Blas�B���C�g���[�V���O���s���ꍇ�A�����BlasVector�ɎQ�Ƃ�ۑ�����B
		bool m_isRaytracingInitialized;							//���C�g���[�V���O�ɕK�v�ȏ�񂪏���������Ă��邩�B
		bool m_isRaytracingEnable;								//���C�g���[�V���O���L��������Ă��邩�B m_isRaytracingInitialized(����������Ă���) and m_isRaytracingEnable(�L��������Ă���) ����TLAS�ɕۑ������B ���X�^���C�Y�̕`��͍s�����ǃ��C�g���͈�U�؂�ꍇ�Ɏg�p���邱�Ƃ��l�����č쐬���܂����B�����ɂ���ăJ�����O�����肷��ꍇ�ł��ˁI
	};

	//DrawFunc���g�p����ۂɕK�v�ȃf�[�^
	struct DrawCallData
	{
		DrawCallData(std::source_location location = std::source_location::current()) :
			callLocation(location), renderTargetHandle(-1)
		{
		};
		/// <summary>
		/// ���C�g���[�V���O������
		/// </summary>
		/// <param name="arg_isOpaque"> ���̃I�u�W�F�N�g�͕s�������HAnyhitShader���Ă΂�邩�ǂ����������Ŕ��f����B </param>
		void SetupRaytracing(bool arg_isOpaque);
		//���_���
		KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
		KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;
		KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData drawMultiMeshesIndexInstanceCommandData;
		VERT_TYPE drawCommandType;
		std::vector<std::vector<KazBufferHelper::BufferData>> materialBuffer;

		//���C�g���[�V���O�Ɏg�p������
		RaytracingData m_raytracingData;

		//���_��񂪊i�[����Ă���f�[�^�̃n���h��
		RESOURCE_HANDLE m_modelVertDataHandle;

		RESOURCE_HANDLE renderTargetHandle;
		//�p�C�v���C�����
		PipelineGenerateData pipelineData;

		//���̑��K�v�ȃo�b�t�@�̐ݒ�
		std::vector<KazBufferHelper::BufferData>extraBufferArray;

		std::source_location callLocation;
	};

	//�P�F�̃|���S���\��(�C���f�b�N�X�Ȃ�)
	static DrawCallData SetDrawPolygonData(const KazRenderHelper::DrawInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//���_���
		lDrawCallData.drawInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::INSTANCE;

		//�s����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�F���
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�p�C�v���C�����̃Z�b�g
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//�P�F�̃|���S���\��(�C���f�b�N�X����)
	static DrawCallData SetDrawPolygonIndexData(const KazRenderHelper::DrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA, std::source_location location = std::source_location::current())
	{
		DrawCallData lDrawCallData;
		//���_���
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::INDEX;

		//�s����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�F���
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT4))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�p�C�v���C�����̃Z�b�g
		lDrawCallData.pipelineData = PIPELINE_DATA;

		lDrawCallData.callLocation = location;

		return lDrawCallData;
	};

	//OBJ���f���̃|���S���\��(�C���f�b�N�X����)
	static DrawCallData SetDrawOBJIndexData(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData& VERTEX_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//���_���
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�}�e���A�����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(MaterialBufferData))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//�p�C�v���C�����̃Z�b�g
		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//���f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������)
	static DrawCallData SetDrawGLTFIndexMaterialData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;

		lDrawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		lDrawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetBuffer(MODEL_DATA.modelVertDataHandle).index;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto& obj : MODEL_DATA.modelData)
		{
			lDrawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//�s����
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

	//���f���̃|���S���\��(�C���f�b�N�X����A�}�e���A������)
	static DrawCallData SetDrawGLTFIndexMaterialInRayTracingData(const ModelInfomation &MODEL_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;

		lDrawCallData.pipelineData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();

		//���_���
		lDrawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetBuffer(MODEL_DATA.modelVertDataHandle).index;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
		for (auto &obj : MODEL_DATA.modelData)
		{
			lDrawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
		}

		//�s����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(CoordinateSpaceMatData))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;
		lDrawCallData.extraBufferArray.back().structureSize = sizeof(CoordinateSpaceMatData);

		//���C�g�����ł̔��f
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


	//�s����̂�
	static DrawCallData SetTransformData(const KazRenderHelper::DrawIndexInstanceCommandData &VERTEX_DATA, const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;
		//���_���
		lDrawCallData.drawIndexInstanceCommandData = VERTEX_DATA;
		lDrawCallData.drawCommandType = VERT_TYPE::INDEX;

		//�s����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		lDrawCallData.pipelineData = PIPELINE_DATA;

		return lDrawCallData;
	};

	//�s����ƃe�N�X�`��
	static DrawCallData SetTexPlaneData(const PipelineGenerateData &PIPELINE_DATA)
	{
		DrawCallData lDrawCallData;

		RESOURCE_HANDLE handle = VertexBufferMgr::Instance()->GeneratePlaneBuffer();
		//���_���
		lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetBuffer(handle).index;
		lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;

		//�s����
		lDrawCallData.extraBufferArray.emplace_back(
			KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMMATRIX))
		);
		lDrawCallData.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		lDrawCallData.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA;

		//�e�N�X�`�����
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

		//���̑��ݒ�
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