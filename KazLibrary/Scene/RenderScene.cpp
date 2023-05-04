#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	{
		/*	DrawFunc::PipelineGenerateData lData;
			lData.desc = DrawFuncPipelineData::SetTest();
			lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
			lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "DefferdRender.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

			testRArray[0] = std::make_unique<DrawFunc::KazRender>(
				DrawFunc::SetDrawPolygonIndexData(&rasterizeRenderer, boxR.drawIndexInstanceCommandData, lData)
				);

			float lBufferSize = 1280 * 720 * sizeof(DirectX::XMFLOAT4);
			testRArray[0]->GetDrawData()->buffer.emplace_back(
				KazBufferHelper::SetGPUBufferData(lBufferSize)
			);*/
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

	{
		DrawFunc::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);

		testRArray[2] = std::make_unique<DrawFunc::KazRender>(
			DrawFunc::SetTransformData(&rasterizeRenderer, spriteR.drawIndexInstanceCommandData, lData)
			);

		textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::TestPath + "tex.png");

		testRArray[2]->GetDrawData()->buffer.emplace_back(textureBuffer);
		testRArray[2]->GetDrawData()->buffer[1]->rootParamType = GRAPHICS_PRAMTYPE_TEX;
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

	rasterizeRenderer.Update();

}

void RenderScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer(BG_COLOR);

	testRArray[1]->DrawCall(transformArray[1], colorArray[1], 0, motherMat);
	testRArray[2]->DrawTexPlane(transformArray[2], colorArray[2], 0, motherMat);

	rasterizeRenderer.Draw();


	KazMath::Vec2<float>texSize(
		static_cast<float>(textureBuffer->bufferWrapper.GetBuffer()->GetDesc().Width),
		static_cast<float>(textureBuffer->bufferWrapper.GetBuffer()->GetDesc().Height)
	);
	D3D12_GPU_DESCRIPTOR_HANDLE handle = DescriptorHeapMgr::Instance()->GetGpuDescriptorView(textureBuffer->GetViewHandle());
	ImGui::Begin("RenderDebugInfomation");
	ImGui::Text("aaa");
	ImGui::Image((ImTextureID)handle.ptr, ImVec2(texSize.x, texSize.y));
	ImGui::End();

}

int RenderScene::SceneChange()
{
	return 0;
}
