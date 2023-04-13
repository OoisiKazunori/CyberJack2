#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/Pipeline/Shader.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Render/BoxPolygonRender.h"
#include"../Game/Helper/CameraWork.h"

class RenderScene :public SceneBase
{
public:
	RenderScene();
	~RenderScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	DrawingByRasterize rasterizeRenderer;

	std::array<std::unique_ptr<DrawFunc::KazRender>, 3> testRArray;
	std::array<KazMath::Transform3D, 3> transformArray;
	std::array<KazMath::Color, 3> colorArray;
	DirectX::XMMATRIX motherMat;

	BoxPolygonRender boxR;

	CameraWork camera;

	std::array<std::unique_ptr<DrawExcuteIndirect>, 2> gpuParticleRender;
	std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, 2> rootSignatureArray;



	std::unique_ptr<KazRenderHelper::ID3D12ResourceWrapper> vertexBuffer, indexBuffer;

	KazRenderHelper::ID3D12ResourceWrapper uavMatBuffer;


	void GenerateRect(std::array<SpriteVertex, 4>&lVerticesArray, std::array<USHORT, 6> &lIndicesArray)
	{
		lIndicesArray = KazRenderHelper::InitIndciesForPlanePolygon();
		KazRenderHelper::InitVerticesPos(&lVerticesArray[0].pos, &lVerticesArray[1].pos, &lVerticesArray[2].pos, &lVerticesArray[3].pos, { 0.5f,0.5f });
		KazRenderHelper::InitUvPos(&lVerticesArray[0].uv, &lVerticesArray[1].uv, &lVerticesArray[2].uv, &lVerticesArray[3].uv);

		BUFFER_SIZE lVertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVerticesArray.size(), sizeof(SpriteVertex));
		BUFFER_SIZE lIndexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndicesArray.size(), sizeof(USHORT));


		vertexBuffer = std::make_unique<KazRenderHelper::ID3D12ResourceWrapper>();
		indexBuffer = std::make_unique<KazRenderHelper::ID3D12ResourceWrapper>();


		vertexBuffer->CreateBuffer(KazBufferHelper::SetVertexBufferData(lVertBuffSize));
		indexBuffer->CreateBuffer(KazBufferHelper::SetIndexBufferData(lIndexBuffSize));

		vertexBuffer->TransData(lVerticesArray.data(), lVertBuffSize);
		indexBuffer->TransData(lIndicesArray.data(), lIndexBuffSize);
	}

	void GeneratePipeline()
	{
		//��
		D3D12_RENDER_TARGET_BLEND_DESC alphaBlendDesc{};
		alphaBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		alphaBlendDesc.BlendEnable = true;
		alphaBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
		alphaBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
		alphaBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;

		alphaBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
		alphaBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
		alphaBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

		{
			D3D12_INPUT_ELEMENT_DESC *input2DLayOut = new D3D12_INPUT_ELEMENT_DESC[2];

			input2DLayOut[0] =
			{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0 };


			input2DLayOut[1] =
			{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
			};


			//�p�C�v���C���̐ݒ�
			D3D12_GRAPHICS_PIPELINE_STATE_DESC gPipeline{};
			//�T���v���}�X�N
			gPipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
			//���X�^���C�U
			//�w�ʃJ�����O�A�h��Ԃ��A�[�x�N���b�s���O�L��
			CD3DX12_RASTERIZER_DESC rasterrize(D3D12_DEFAULT);
			rasterrize.CullMode = D3D12_CULL_MODE_NONE;
			gPipeline.RasterizerState = rasterrize;
			//�u�����h���[�h
			gPipeline.BlendState.RenderTarget[0] = alphaBlendDesc;

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



			RootSignatureDataTest lRootsignature;
			lRootsignature.rangeArray.push_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA));
			lRootsignature.rangeArray.push_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_SRV_DESC, GRAPHICS_PRAMTYPE_DATA2));
			rootSignatureArray[1] = GraphicsRootSignature::Instance()->CreateRootSignature(lRootsignature, ROOTSIGNATURE_GRAPHICS);

			ShaderOptionData vs(KazFilePathName::ShaderFilePath + "GPUParticle.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
			ShaderOptionData ps(KazFilePathName::ShaderFilePath + "GPUParticle.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

			GraphicsPipeLineMgr::Instance()->CreatePipeLine(
				input2DLayOut,
				2,
				vs,
				ps,
				gPipeline,
				rootSignatureArray[1],
				PIPELINE_NAME_GPUPARTICLE_TEXCOLOR
			);
		}
	}

	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};
};

