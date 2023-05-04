#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	//G-Bufferに書き込む予定のオブジェクト
	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTest();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

		testRArray[0] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
			);

		//Albedo用のG-Bufferを生成
		int lBufferSize = 1280 * 720 * sizeof(DirectX::XMFLOAT4);
		testRArray[0]->GetDrawData()->buffer.emplace_back(
			std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetRWStructuredBuffer(lBufferSize, "G-Buffer_Write"))
		);
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();
		DescriptorHeapMgr::Instance()->CreateBufferView(view, KazBufferHelper::SetUnorderedAccessView(sizeof(DirectX::XMFLOAT4), 1280 * 720), testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer().Get());
		testRArray[0]->GetDrawData()->buffer[2]->CreateViewHandle(view);
		testRArray[0]->GetDrawData()->buffer[2]->elementNum = 1280 * 720;
		testRArray[0]->GetDrawData()->buffer[2]->rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		testRArray[0]->GetDrawData()->buffer[2]->rootParamType = GRAPHICS_PRAMTYPE_DATA3;
	}

	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTest();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "TestDraw.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "TestDraw.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

		testRArray[1] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
			);
	}

	//G-Bufferの描画確認
	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DrawGBuffer.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

		testRArray[2] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetTransformData(&rasterizeRenderer, spriteR.drawIndexInstanceCommandData, lData)
			);

		//Albedo用のG-Bufferを生成
		int lBufferSize = 1280 * 720 * sizeof(DirectX::XMFLOAT4);
		testRArray[2]->GetDrawData()->buffer.emplace_back(
			std::make_shared<KazBufferHelper::BufferData>(KazBufferHelper::SetRWStructuredBuffer(lBufferSize, "G-Buffer_Read"))
		);
		RESOURCE_HANDLE view = UavViewHandleMgr::Instance()->GetHandle();
		DescriptorHeapMgr::Instance()->CreateBufferView(view, KazBufferHelper::SetUnorderedAccessView(sizeof(DirectX::XMFLOAT4), 1280 * 720), testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer().Get());
		testRArray[2]->GetDrawData()->buffer[1]->CreateViewHandle(view);
		testRArray[2]->GetDrawData()->buffer[1]->elementNum = 1280 * 720;
		testRArray[2]->GetDrawData()->buffer[1]->rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		testRArray[2]->GetDrawData()->buffer[1]->rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	}
	transformArray[0].pos = { 0.0f,0.0f,0.0f };
	transformArray[1].pos = { 10.0f,0.0f,0.0f };
	transformArray[2].pos = { 20.0f,0.0f,0.0f };

	colorArray[0] = { 155,155,155,255 };
	colorArray[1] = { 155,0,0,155 };
	colorArray[2] = { 0,155,0,55 };

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


	testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

	testRArray[2]->GetDrawData()->buffer[1]->bufferWrapper.CopyBuffer(
		testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.ChangeBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);


	rasterizeRenderer.Update();

}

void RenderScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	testRArray[0]->DrawCall(transformArray[0], colorArray[0], 0, motherMat);
	testRArray[1]->DrawCall(transformArray[1], colorArray[1], 0, motherMat);
	testRArray[2]->DrawTexPlane(transformArray[2], colorArray[2], 0, motherMat);

	rasterizeRenderer.Draw();


	KazMath::Vec2<float>texSize(
		static_cast<float>(testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer()->GetDesc().Width),
		static_cast<float>(testRArray[0]->GetDrawData()->buffer[2]->bufferWrapper.GetBuffer()->GetDesc().Height)
	);
	//D3D12_GPU_DESCRIPTOR_HANDLE handle = DescriptorHeapMgr::Instance()->GetGpuDescriptorView(testRArray[0]->GetDrawData()->buffer[2]->GetViewHandle());
	//ImGui::Begin("RenderDebugInfomation");
	//ImGui::Text("aaa");
	//ImGui::Image((ImTextureID)handle.ptr, ImVec2(texSize.x, texSize.y));
	//ImGui::End();

}

int RenderScene::SceneChange()
{
	return 0;
}
