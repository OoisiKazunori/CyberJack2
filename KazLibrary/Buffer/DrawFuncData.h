#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"

namespace DrawFuncPipelineData
{
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

		D3D12_INPUT_ELEMENT_DESC *input3DLayOut = new D3D12_INPUT_ELEMENT_DESC
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		};
		gPipeline.InputLayout.pInputElementDescs = input3DLayOut;
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

		D3D12_INPUT_ELEMENT_DESC *input3DLayOut = new D3D12_INPUT_ELEMENT_DESC[2];
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

