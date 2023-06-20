#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Buffer/GBufferMgr.h"
#include"../KazLibrary/Render/DrawFunc.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	boxData = boxBuffer.GenerateBoxBuffer(1.0f);

	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 3;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 1, 1, 1, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);

	//G-Buffer生成
	GBufferMgr::Instance();

	model = ModelLoader::Instance()->Load("Resource/Test/glTF/", "sponza.gltf");

	//フォワードレンダリングで描画するモデル
	{
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

		//描画
		drawSponza = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(*model, lData);
		//その他バッファ
		drawSponza.extraBufferArray.emplace_back(KazBufferHelper::BufferData(KazBufferHelper::SetConstBufferData(sizeof(DirectX::XMFLOAT3))));
		drawSponza.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		drawSponza.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA3;
		drawSponza.extraBufferArray.back().structureSize = sizeof(DirectX::XMFLOAT3);

		drawSponza.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];

		//レイトレの準備
		drawSponza.SetupRaytracing(true);
	}

	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;

		for (int i = 0; i < m_drawPlaneArray.size(); ++i)
		{
			m_drawPlaneArray[i].m_drawFlag = false;
			m_drawPlaneArray[i].m_plane = DrawFuncData::SetTexPlaneData(lData);
		}
		m_drawPlaneArray[0].m_bufferName = "Albedo";
		m_drawPlaneArray[1].m_bufferName = "Normal";
		m_drawPlaneArray[2].m_bufferName = "MetalnessRoughness";
		m_drawPlaneArray[3].m_bufferName = "World";
	}
	{
		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetTex();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;

		m_drawFinalPlane.m_plane = DrawFuncData::SetTexPlaneData(lData);
		m_drawFinalPlane.m_bufferName = "Final";
	}



	transformArray[0].pos = { 0.0f,0.0f,0.0f };
	transformArray[1].pos = { 10.0f,0.0f,0.0f };
	transformArray[1].scale = { 1.0f,1.0f,1.0f };

	transformArray[2].pos = { 1280.0f,720.0f,0.0f };
	transformArray[2].scale = { 0.25f,0.25f,0.0f };
	transformArray[3].pos = { 1280.0f,525.0f,0.0f };
	transformArray[3].scale = { 0.25f,0.25f,0.0f };
	transformArray[4].pos = { 1280.0f,300.0f,0.0f };
	transformArray[4].scale = { 0.25f,0.25f,0.0f };


	colorArray[0] = { 155,155,155,255 };
	colorArray[1] = { 155,0,0,155 };
	colorArray[2] = { 0,155,0,55 };

	texFlag = true;


	//clearGBuffer.SetBuffer(testRArray[0]->GetDrawData()->buffer[2], GRAPHICS_PRAMTYPE_DATA);


	//ライティングパス
	{
		DispatchComputeShader::ComputeData computeData;
		//設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		computeData.desc = desc;
		//シェーダーのパス
		computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "DefferdRenderLightingPass.hlsl", "CSLightingPass", "cs_6_4", SHADER_TYPE_COMPUTE);

		//ディスパッチのアドレス
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		computeData.dispatchData = &dispatchData;

		gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		finalGBuffer.rootParamType = GRAPHICS_PRAMTYPE_DATA3;

		//セットするバッファ
		computeData.bufferArray =
		{
			finalGBuffer,
			gBuffer[0],
			gBuffer[1]
		};

		//積む
		compute.Stack(computeData);
	}

	//クリア処理
	{
		DispatchComputeShader::ComputeData computeData;
		//設定
		D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
		desc.NodeMask = 0;
		computeData.desc = desc;
		//シェーダーのパス
		computeData.shaderData = ShaderOptionData(KazFilePathName::ComputeShaderPath + "ClearGBuffer.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE);

		//ディスパッチのアドレス
		dispatchData.x = 1280;
		dispatchData.y = 720;
		dispatchData.z = 1;
		computeData.dispatchData = &dispatchData;

		gBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		gBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA2;

		//セットするバッファ
		computeData.bufferArray =
		{
			gBuffer[0],
			gBuffer[1]
		};

		//積む
		compute.Stack(computeData);
	}



}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
	camera.Init({});
	lightVec = { 0.0f,1.0f,0.0f };
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


	DrawFunc::DrawModelInRaytracing(drawSponza, transformArray[0], DrawFunc::REFRACTION);
	DirectX::XMFLOAT3 dir = lightVec.ConvertXMFLOAT3();
	drawSponza.extraBufferArray[2].bufferWrapper->TransData(&dir, sizeof(DirectX::XMFLOAT3));


	{
		KazMath::Transform2D transform({ 1280.0f,720.0f }, { 1280.0f,720.0f });

		for (int i = 0; i < m_drawPlaneArray.size(); ++i)
		{
			if (!m_drawPlaneArray[i].m_drawFlag)
			{
				continue;
			}
			RESOURCE_HANDLE handle = GBufferMgr::Instance()->GetRenderTarget()[i];
			DrawFunc::DrawTextureIn2D(m_drawPlaneArray[i].m_plane, transform, RenderTargetStatus::Instance()->GetBuffer(handle));
		}

		//合成結果
		RESOURCE_HANDLE finaleHandle = GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::FINAL];
		DrawFunc::DrawTextureIn2D(m_drawFinalPlane.m_plane, transform, RenderTargetStatus::Instance()->GetBuffer(finaleHandle));
	}
	//法線描画
	//compute.Update();

	//Blasの配列をクリア
	m_blasVector.Update();

}

void RenderScene::Draw()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	rasterizeRenderer.ObjectRender(drawSponza);
	for (int index = 0; index < static_cast<int>(drawSponza.m_raytracingData.m_blas.size()); ++index) {
		m_blasVector.Add(drawSponza.m_raytracingData.m_blas[index], transformArray[0].GetMat());
	}


	for (int i = 0; i < m_drawPlaneArray.size(); ++i)
	{
		if (!m_drawPlaneArray[i].m_drawFlag)
		{
			continue;
		}
		rasterizeRenderer.ObjectRender(m_drawPlaneArray[i].m_plane);
	}
	//最終合成結果
	if (m_drawFinalPlane.m_drawFlag)
	{
		rasterizeRenderer.ObjectRender(m_drawFinalPlane.m_plane);
	}

	rasterizeRenderer.Sort();
	//compute.Compute();
	rasterizeRenderer.Render();


	/*----- レイトレ描画開始 -----*/

	//Tlasを構築 or 再構築する。
	m_tlas.Build(m_blasVector);

	//レイトレ用のデータを構築。
	m_rayPipeline->BuildShaderTable(m_blasVector);

	//レイトレ実行。
	m_rayPipeline->TraceRay(m_tlas);


	/*----- レイトレ描画終了 -----*/


	ImGui::Begin("Light");
	ImGui::DragFloat("VecX", &lightVec.x);
	ImGui::DragFloat("VecY", &lightVec.y);
	ImGui::DragFloat("VecZ", &lightVec.z);
	for (auto& obj : m_drawPlaneArray)
	{
		ImGui::Checkbox(obj.m_bufferName.c_str(), &obj.m_drawFlag);
	}
	ImGui::Checkbox(m_drawFinalPlane.m_bufferName.c_str(), &m_drawFinalPlane.m_drawFlag);
	ImGui::End();
}

int RenderScene::SceneChange()
{
	return 0;
}
