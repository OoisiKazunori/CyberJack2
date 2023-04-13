#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	DrawFunc::PipelineGenerateData lData;
	lData.desc = DrawFuncPipelineData::SetTest();
	lData.shaderDataArray.emplace_back(KazFilePathName::VertexShaderPath + "TestDraw.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	lData.shaderDataArray.emplace_back(KazFilePathName::VertexShaderPath + "TestDraw.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

	testRArray[0] = std::make_unique<DrawFunc::KazRender>(
		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
		);

	testRArray[1] = std::make_unique<DrawFunc::KazRender>(
		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
		);

	testRArray[2] = std::make_unique<DrawFunc::KazRender>(
		DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
		);

	transformArray[0].pos = { 0.0f,0.0f,0.0f };
	transformArray[1].pos = { 10.0f,0.0f,0.0f };
	transformArray[2].pos = { 20.0f,0.0f,0.0f };

	colorArray[0] = { 155,155,155,255 };
	colorArray[1] = { 155,0,0,155 };
	colorArray[2] = { 0,155,0,55 };

	std::array<SpriteVertex, 4>lVerticesArray;
	std::array<USHORT, 6> lIndicesArray;

	BUFFER_SIZE lVertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVerticesArray.size(), sizeof(SpriteVertex));
	BUFFER_SIZE lIndexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndicesArray.size(), sizeof(UINT));

	KazBufferHelper::BufferResourceData lBufferData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		CD3DX12_RESOURCE_DESC::Buffer(sizeof(OutputData)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		"CopyCounterBuffer"
	);
	uavMatBuffer.CreateBuffer(lBufferData);

	//�p�[�e�B�N���̒��_���
	GenerateRect(lVerticesArray, lIndicesArray);

	//�e�N�X�`���t���p�[�e�B�N���̃p�C�v���C��
	GeneratePipeline();
	{
		//���[�g�V�O�l�`���[��V�����p�ӂ���
		//RootSignatureDataTest lRootsignature;
		//lRootsignature.rangeArray.push_back(BufferRootsignature(GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA));
		//rootSignatureArray[0] = GraphicsRootSignature::Instance()->CreateRootSignature(lRootsignature, ROOTSIGNATURE_GRAPHICS);
		//�����炠�������p��(�g�p����O���t�B�b�N�p�C�v���C���ƈꏏ)
		rootSignatureArray[0] = GraphicsRootSignature::Instance()->GetRootSignature(ROOTSIGNATURE_DATA_DRAW_UAV);

		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> args;
		//UAV���
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
		args[0].UnorderedAccessView.RootParameterIndex = 0;
		//�C���f�b�N�X����
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;


		InitDrawIndexedExcuteIndirect data;
		data.vertexBufferView = KazBufferHelper::SetVertexBufferView(gpuVertexBuffer.GetGpuAddress(), lVertBuffSize, sizeof(lVerticesArray[0]));
		data.indexBufferView = KazBufferHelper::SetIndexBufferView(gpuIndexBuffer.GetGpuAddress(), lIndexBuffSize);
		data.indexNum = static_cast<UINT>(6);
		data.elementNum = 1;
		data.updateView = uavMatBuffer.GetBuffer()->GetGPUVirtualAddress();
		data.rootsignature = rootSignatureArray[0];
		data.texHandle = -1;
		data.argument = args;


		gpuParticleRender[0] = std::make_unique<DrawExcuteIndirect>(data);
	}

	{
		std::vector<D3D12_INDIRECT_ARGUMENT_DESC> args;
		//UAV
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[args.size() - 1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_UNORDERED_ACCESS_VIEW;
		args[args.size() - 1].UnorderedAccessView.RootParameterIndex = 0;
		//SRV
		//args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		//args[args.size() - 1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_SHADER_RESOURCE_VIEW;
		//args[args.size() - 1].ShaderResourceView.RootParameterIndex = 1;
		//�C���f�b�N�X����
		args.emplace_back(D3D12_INDIRECT_ARGUMENT_DESC());
		args[args.size() - 1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;


		InitDrawIndexedExcuteIndirect data;
		data.vertexBufferView = KazBufferHelper::SetVertexBufferView(gpuVertexBuffer.GetGpuAddress(), lVertBuffSize, sizeof(lVerticesArray[0]));
		data.indexBufferView = KazBufferHelper::SetIndexBufferView(gpuIndexBuffer.GetGpuAddress(), lIndexBuffSize);
		data.indexNum = static_cast<UINT>(6);
		data.elementNum = 1;
		data.texHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "tex.png");
		data.updateView = uavMatBuffer.GetBuffer()->GetGPUVirtualAddress();
		data.rootsignature = rootSignatureArray[1];
		data.argument = args;

		gpuParticleRender[1] = std::make_unique<DrawExcuteIndirect>(data);
	}
}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
	camera.Init({});
}

void RenderScene::PreInit()
{
}

void RenderScene::Finalize()
{
}

void RenderScene::Input()
{
}

void RenderScene::Update()
{
	camera.Update({}, {}, true);
	CameraMgr::Instance()->Camera(camera.GetEyePos(), camera.GetTargetPos(), { 0.0f,1.0f,0.0f });

	rasterizeRenderer.Update();


	{
		OutputData data;
		data.color = { 1.0f,1.0f,1.0f,1.0f };
		data.mat = CameraMgr::Instance()->GetViewMatrix() * CameraMgr::Instance()->GetPerspectiveMatProjection();
		uavMatBuffer.TransData(&data, sizeof(OutputData));
	}
}

void RenderScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	for (int i = 0; i < testRArray.size(); ++i)
	{
		testRArray[i]->DrawCall(transformArray[i], colorArray[i], 0, motherMat);
	}


	//rasterizeRenderer.Draw();


	//gpuParticleRender[0]->Draw(PIPELINE_NAME_GPUPARTICLE, nullptr);
	gpuParticleRender[1]->Draw(PIPELINE_NAME_GPUPARTICLE_TEXCOLOR, nullptr);

}

int RenderScene::SceneChange()
{
	return 0;
}
