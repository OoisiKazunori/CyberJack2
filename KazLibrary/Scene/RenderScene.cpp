#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

RenderScene::RenderScene()
{
	endGameFlag = false;


	boxData = boxBuffer.GenerateBoxBuffer(1.0f);

	//G-Buffer�ɏ������ޗ\��̃I�u�W�F�N�g
	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTest();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);


		testRArray[0] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxData.index, lData)
			);

		//Albedo�p��G-Buffer�𐶐�
		int lBufferSize = 1280 * 720;
		testRArray[0]->GetDrawData()->buffer.emplace_back(
			std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetUAVTexBuffer(1280, 720, "G-Buffer_Write"))
		);
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = 1280 * 720;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;


		DescriptorHeapMgr::Instance()->CreateBufferView(view, uavDesc, testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer().Get());
		testRArray[0]->GetDrawData()->buffer[2]->CreateViewHandle(view);
		testRArray[0]->GetDrawData()->buffer[2]->elementNum = 1280 * 720;
		testRArray[0]->GetDrawData()->buffer[2]->rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		testRArray[0]->GetDrawData()->buffer[2]->rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	}

	//�t�H���[�h�����_�����O�ŕ`�悷�闧����
	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTest();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "TestDraw.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "TestDraw.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

		testRArray[1] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxData.index, lData)
			);
	}

	//G-Buffer�̕`��m�F�p�̔|��
	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

		planeData = texBuffer.GeneratePlaneTexBuffer(
			{
				1.0f,
				1.0f
			},
			{
				static_cast<int>(testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer()->GetDesc().Width),
				static_cast<int>(testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer()->GetDesc().Height)
			}
		);

		testRArray[2] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetTransformData(&rasterizeRenderer, planeData.index, lData)
			);

		//Albedo�p��G-Buffer�𐶐�
		testRArray[2]->GetDrawData()->buffer.emplace_back(
			testRArray[0]->GetDrawData()->buffer[2]
		);
	}
	transformArray[0].pos = { 0.0f,0.0f,0.0f };
	transformArray[1].pos = { 10.0f,0.0f,0.0f };
	transformArray[2].pos = { 1280.0f,720.0f,0.0f };
	transformArray[2].scale = { 0.25f,0.25f,0.0f };


	colorArray[0] = { 155,155,155,255 };
	colorArray[1] = { 155,0,0,155 };
	colorArray[2] = { 0,155,0,55 };

	texFlag = true;


	//clearGBuffer.SetBuffer(testRArray[0]->GetDrawData()->buffer[2], GRAPHICS_PRAMTYPE_DATA);

	//�R���s���[�g�V�F�[�_�[�̐���
	{
		DispatchComputeShader::ComputeData computeData;
		//�ݒ�
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		computeData.desc = desc;
		//�V�F�[�_�[�̃p�X
		computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "ClearGBuffer.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE);

		//�f�B�X�p�b�`�̃A�h���X
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		computeData.dispatchData = &dispatchData;

		//�Z�b�g����o�b�t�@
		computeData.bufferArray =
		{
			{testRArray[0]->GetDrawData()->buffer[2]}
		};

		//�ς�
		compute.Stack(computeData);
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
	compute.Update();
}

void RenderScene::Draw()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	testRArray[0]->DrawCall(transformArray[0], colorArray[0], 0, motherMat);
	testRArray[1]->DrawCall(transformArray[1], colorArray[1], 0, motherMat);
	testRArray[2]->DrawTexPlane(transformArray[2], colorArray[2], 0, motherMat);

	compute.Compute();
	rasterizeRenderer.Draw();

}

int RenderScene::SceneChange()
{
	return 0;
}
