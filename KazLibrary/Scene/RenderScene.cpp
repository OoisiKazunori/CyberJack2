#include "RenderScene.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Buffer/GBufferMgr.h"

RenderScene::RenderScene()
{
	endGameFlag = false;

	m_boxData = m_boxBuffer.GenerateBoxBuffer(1.0f);

	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 4;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 5, 3, 2, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);

	//G-Buffer生成
	GBufferMgr::Instance();



	m_model = ModelLoader::Instance()->Load("Resource/Test/glTF/Sponza/", "sponza.gltf");
	m_refractionModel = ModelLoader::Instance()->Load("Resource/Test/", "refraction.gltf");

	m_testModelArray[0] = ModelLoader::Instance()->Load("Resource/Test/glTF/Avocado/", "Avocado.gltf");
	m_testModelArray[1] = ModelLoader::Instance()->Load("Resource/Test/glTF/BoomBox/", "BoomBox.gltf");
	m_testModelArray[2] = ModelLoader::Instance()->Load("Resource/Test/glTF/Corset/", "Corset.gltf");
	m_testModelArray[3] = ModelLoader::Instance()->Load("Resource/Test/glTF/WaterBottle/", "WaterBottle.gltf");
	m_testModelArray[4] = ModelLoader::Instance()->Load("Resource/Test/glTF/Suzanne/", "Suzanne.gltf");
	m_testModelArray[5] = ModelLoader::Instance()->Load("Resource/Test/glTF/BarramundiFish/", "BarramundiFish.gltf");
	//m_testModelArray[6] = ModelLoader::Instance()->Load("Resource/Test/glTF/Lantern/", "Lantern.gltf");
	//m_testModelArray[7] = ModelLoader::Instance()->Load("Resource/Test/glTF/SciFiHelmet/", "SciFiHelmet.gltf");


	//モデル描画
	{
		//スポンザ
		m_drawSponza = DrawFuncData::SetDefferdRenderingModel(m_model);


		std::array<float, 6>scaleArray({ 1500.0f, 1500.0f, 1500.0f, 500.0f, 50.0f,500.0f });
		m_testModelFiledArray.emplace_back(TestModelField(m_testModelArray, scaleArray));
		m_testModelFiledArray.emplace_back(TestModelField(m_testModelArray, scaleArray));

		//球の描画
		m_refractionSphere = DrawFuncData::SetDefferdRenderingModel(m_refractionModel);



		DrawFuncData::PipelineGenerateData lData;
		lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvLightMain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvLightMain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;

		m_alphaModel = DrawFuncData::SetDrawGLTFIndexMaterialData(*m_refractionModel, lData);
		KazMath::Color color(255, 255, 255, 255 / 2);
		m_alphaModel.extraBufferArray[1].bufferWrapper->TransData(&color.ConvertColorRateToXMFLOAT4(), sizeof(DirectX::XMFLOAT4));
		m_alphaModel.renderTargetHandle = GBufferMgr::Instance()->GetRenderTarget()[0];
		m_alphaModel.depthHandle = 0;
		m_alphaModelTransform.scale = { 100.0f,100.0f,100.0f };
	}

	//ライトの情報
	{
		for (int y = 0; y < m_lightTransformArray.size(); ++y)
		{
			for (int x = 0; x < m_lightTransformArray[y].size(); ++x)
			{
				for (int z = 0; z < m_lightTransformArray[y][x].size(); ++z)
				{
					m_lightBoxDataArray[y][x][z] = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Test/glTF/Box/", "BoxTextured.gltf"));
					KazMath::Vec3<float> pos(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
					float xInterval = 400.0f;
					float zInterval = 300.0f;
					float height = 150.0f;
					m_lightTransformArray[y][x][z].pos = { -800.0f + pos.x * xInterval,pos.y * height,-500.0f + pos.z * zInterval };
					m_lightTransformArray[y][x][z].scale = { 10.0f,10.0f,10.0f };
				}
			}
		}
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
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "GBufferDrawFinal.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
		lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "GBufferDrawFinal.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
		lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;

		m_drawFinalPlane.m_plane = DrawFuncData::SetTexPlaneData(lData);
		m_drawFinalPlane.m_plane.extraBufferArray.emplace_back();
		m_drawFinalPlane.m_plane.extraBufferArray.emplace_back();
		m_drawFinalPlane.m_plane.extraBufferArray.emplace_back(KazBufferHelper::SetConstBufferData(sizeof(int)));
		m_drawFinalPlane.m_plane.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		m_drawFinalPlane.m_plane.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		m_drawFinalPlane.m_plane.extraBufferArray.emplace_back();
		m_drawFinalPlane.m_bufferName = "Final";

		lightUploadBuffer = KazBufferHelper::SetUploadBufferData(sizeof(DirectX::XMFLOAT3) * (LGHIT_ARRAY_X * LGHIT_ARRAY_Y * LGHIT_ARRAY_Z));

		std::vector<DirectX::XMFLOAT3>posArray;
		for (int y = 0; y < m_lightTransformArray.size(); ++y)
		{
			for (int x = 0; x < m_lightTransformArray[y].size(); ++x)
			{
				for (int z = 0; z < m_lightTransformArray[y][x].size(); ++z)
				{
					posArray.emplace_back(DirectX::XMFLOAT3{ m_lightTransformArray[y][x][z].pos.x, m_lightTransformArray[y][x][z].pos.y, m_lightTransformArray[y][x][z].pos.z });
				}
			}
		}
		lightUploadBuffer.bufferWrapper->TransData(posArray.data(), sizeof(DirectX::XMFLOAT3) * (LGHIT_ARRAY_X * LGHIT_ARRAY_Y * LGHIT_ARRAY_Z));

		m_drawFinalPlane.m_plane.extraBufferArray.emplace_back(KazBufferHelper::SetGPUBufferData(sizeof(DirectX::XMFLOAT3) * (LGHIT_ARRAY_X * LGHIT_ARRAY_Y * LGHIT_ARRAY_Z)));
		m_drawFinalPlane.m_plane.extraBufferArray.back().bufferWrapper->CopyBuffer(lightUploadBuffer.bufferWrapper->GetBuffer(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
		m_drawFinalPlane.m_plane.extraBufferArray.back().rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		m_drawFinalPlane.m_plane.extraBufferArray.back().rootParamType = GRAPHICS_PRAMTYPE_DATA2;
	}



	m_transformArray[0].pos = { 0.0f,0.0f,0.0f };
	m_transformArray[1].pos = { 10.0f,0.0f,0.0f };
	m_transformArray[1].scale = { 1.0f,1.0f,1.0f };

	m_transformArray[2].pos = { 1280.0f,720.0f,0.0f };
	m_transformArray[2].scale = { 0.25f,0.25f,0.0f };
	m_transformArray[3].pos = { 1280.0f,525.0f,0.0f };
	m_transformArray[3].scale = { 0.25f,0.25f,0.0f };
	m_transformArray[4].pos = { 1280.0f,300.0f,0.0f };
	m_transformArray[4].scale = { 0.25f,0.25f,0.0f };


	m_colorArray[0] = { 155,155,155,255 };
	m_colorArray[1] = { 155,0,0,155 };
	m_colorArray[2] = { 0,155,0,55 };

	m_texFlag = true;


	//clearGBuffer.SetBuffer(testRArray[0]->GetDrawData()->buffer[2], GRAPHICS_PRAMTYPE_DATA);



	//クリア処理
	{
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::ALBEDO])
		};
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_dispatch.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "TestCompute.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
	}

	m_raytracingFlag = true;


	//ボリュームテクスチャを生成。
	m_volumeFogTextureBuffer = KazBufferHelper::SetUAV3DTexBuffer(256, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_volumeFogTextureBuffer.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_volumeFogTextureBuffer.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccess3DTextureView(sizeof(DirectX::XMFLOAT4), 256 * 256 * 256),
		m_volumeFogTextureBuffer.bufferWrapper->GetBuffer().Get()
	);
	//ボリュームノイズパラメーター
	m_noiseParamData = KazBufferHelper::SetConstBufferData(sizeof(NoiseParam));
	//ボリュームノイズのパラメーターを設定
	m_noiseParam.m_timer = 0.0f;
	m_noiseParam.m_windSpeed = 10.00f;
	m_noiseParam.m_windStrength = 1.0f;
	m_noiseParam.m_threshold = 0.42f;
	m_noiseParam.m_skydormScale = 356.0f;
	m_noiseParam.m_octaves = 4;
	m_noiseParam.m_persistence = 0.5f;
	m_noiseParam.m_lacunarity = 2.5f;
	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	//ボリュームノイズ書き込み
	{
		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_volumeFogTextureBuffer,
			 m_noiseParamData
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_volumeNoiseShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "Raytracing/" + "Write3DNoise.hlsl", "CSmain", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);

		m_rayPipeline->SetVolumeFogTexture(&m_volumeFogTextureBuffer);
	}

	//レイマーチングのパラメーターを設定。
	m_raymarchingParam.m_pos = KazMath::Vec3<float>();
	m_raymarchingParam.m_color = KazMath::Vec3<float>(1.0f, 1.0f, 1.0f);
	m_raymarchingParam.m_wrapCount = 20.0f;
	m_raymarchingParam.m_gridSize = 4.0f;
	m_raymarchingParam.m_wrapCount = 30.0f;
	m_raymarchingParam.m_density = 0.65f;
	//m_raymarchingParam.m_density = 1.0f;
	m_raymarchingParam.m_sampleLength = 30.0f;
	m_raymarchingParam.m_isSimpleFog = 0;
	m_raymarchingParamData = KazBufferHelper::SetConstBufferData(sizeof(RaymarchingParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//レイマーチングのパラメーター用定数バッファをセット。
	m_rayPipeline->SetRaymarchingConstData(&m_raymarchingParamData);

}

RenderScene::~RenderScene()
{
}

void RenderScene::Init()
{
	m_camera.Init({});
	m_lightVec = { 0.0f,1.0f,0.0f };

	m_atem = { 0.1f,0.1f,0.3f };
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
	m_camera.Update({}, {}, true);
	CameraMgr::Instance()->Camera(m_camera.GetEyePos(), m_camera.GetTargetPos(), { 0.0f,1.0f,0.0f });


	DrawFunc::DrawModelInRaytracing(m_drawSponza, m_transformArray[0], DrawFunc::NONE);

	//レイトレデバッグ用のオブジェクトをセット
	m_sphereTransform = m_transformArray[0];
	float scale = 50.0f;
	m_sphereTransform.pos.y = 50.0f;
	m_sphereTransform.scale = { scale,scale,scale };
	//DrawFunc::DrawModelInRaytracing(m_reflectionSphere, m_sphereTransform, DrawFunc::REFLECTION);
	//m_reflectionSphere.extraBufferArray[2].bufferWrapper->TransData(&dir, sizeof(DirectX::XMFLOAT3));
	DrawFunc::DrawModelInRaytracing(m_refractionSphere, m_sphereTransform, DrawFunc::REFRACTION);


	std::array<float, 6>xArray({ -500.0f , -400.0f, -100.0f, 100.0f, 300.0f,500.0f });
	for (int i = 0; i < m_testModelFiledArray.size(); ++i)
	{
		m_testModelFiledArray[i].SetPos(xArray, 200.0f, 50.0f + static_cast<float>(i) * 500.0f);
	}


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
		RESOURCE_HANDLE albedoHandle = GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::ALBEDO];
		DrawFunc::DrawTextureIn2D(m_drawFinalPlane.m_plane, transform, RenderTargetStatus::Instance()->GetBuffer(albedoHandle));
		m_drawFinalPlane.m_plane.extraBufferArray[2] = RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::NORMAL]);
		m_drawFinalPlane.m_plane.extraBufferArray[2].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		m_drawFinalPlane.m_plane.extraBufferArray[3] = RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::WORLD]);
		m_drawFinalPlane.m_plane.extraBufferArray[3].rootParamType = GRAPHICS_PRAMTYPE_DATA3;


		int num = LGHIT_ARRAY_X * LGHIT_ARRAY_Y * LGHIT_ARRAY_Z;
		m_drawFinalPlane.m_plane.extraBufferArray[4].bufferWrapper->TransData(&num, sizeof(int));
		//最終合成結果を格納する。
		m_drawFinalPlane.m_plane.extraBufferArray[5] = GBufferMgr::Instance()->GetFinalBuffer();
		m_drawFinalPlane.m_plane.extraBufferArray[5].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		m_drawFinalPlane.m_plane.extraBufferArray[5].rootParamType = GRAPHICS_PRAMTYPE_DATA;
	}

	//法線描画
	m_dispatch.Compute({ 1,1,1 });

	//Blasの配列をクリア
	m_blasVector.Update();

	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_0))
	{
		m_particle.GenerateEffect(KazMath::Rand<int>(15, 5));
	}
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_1))
	{
		m_particle.ReleaseEffect(1);
	}

	m_particle.UpdateEmitterAndParticleOnGPU();

	//ノイズ用のタイマーを加算。
	GBufferMgr::Instance()->m_cameraEyePosData.m_noiseTimer += 0.02f;

	//ボリュームノイズを書き込む。
	m_noiseParam.m_timer += 0.001f;
	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	m_volumeNoiseShader.Compute({ static_cast<UINT>(256 / 8), static_cast<UINT>(256 / 8), static_cast<UINT>(256 / 4) });


}

void RenderScene::Draw(DrawingByRasterize& arg_rasterize)
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	m_rasterizeRenderer.ObjectRender(m_drawSponza);
	for (int index = 0; index < static_cast<int>(m_drawSponza.m_raytracingData.m_blas.size()); ++index) {
		m_blasVector.Add(m_drawSponza.m_raytracingData.m_blas[index], m_transformArray[0].GetMat());
	}

	//レイトレデバッグ用のオブジェクトを描画。後々関数にまとめます。
	/*m_rasterizeRenderer.ObjectRender(m_reflectionSphere);
	for (int index = 0; index < static_cast<int>(m_reflectionSphere.m_raytracingData.m_blas.size()); ++index) {
		m_blasVector.Add(m_reflectionSphere.m_raytracingData.m_blas[index], m_sphereTransform.GetMat());
	}*/

	//m_rasterizeRenderer.ObjectRender(m_refractionSphere);
	for (int index = 0; index < static_cast<int>(m_refractionSphere.m_raytracingData.m_blas.size()); ++index) {
		//	m_blasVector.Add(m_refractionSphere.m_raytracingData.m_blas[index], m_transformArray[0].GetMat());
	}

	for (int i = 0; i < m_testModelFiledArray.size(); ++i)
	{
		m_testModelFiledArray[i].Update(m_rasterizeRenderer, m_blasVector);
	}

	if (m_lightFlag)
	{
		//ライトの位置可視化用描画
		for (int y = 0; y < m_lightTransformArray.size(); ++y)
		{
			for (int x = 0; x < m_lightTransformArray[y].size(); ++x)
			{
				for (int z = 0; z < m_lightTransformArray[y][x].size(); ++z)
				{
					DrawFunc::DrawModelInRaytracing(m_lightBoxDataArray[y][x][z], m_lightTransformArray[y][x][z], DrawFunc::NONE);
					m_rasterizeRenderer.ObjectRender(m_lightBoxDataArray[y][x][z]);
				}
			}
		}
	}


	for (int i = 0; i < m_drawPlaneArray.size(); ++i)
	{
		if (!m_drawPlaneArray[i].m_drawFlag)
		{
			continue;
		}
		m_rasterizeRenderer.ObjectRender(m_drawPlaneArray[i].m_plane);
	}
	//最終合成結果
	if (m_drawFinalPlane.m_drawFlag)
	{
		m_rasterizeRenderer.ObjectRender(m_drawFinalPlane.m_plane);
	}

	DrawFunc::DrawModelInRaytracing(m_alphaModel, m_alphaModelTransform, DrawFunc::NONE);
	m_rasterizeRenderer.ObjectRender(m_alphaModel);

	m_rasterizeRenderer.Sort();
	m_rasterizeRenderer.Render();


	/*----- レイトレ描画開始 -----*/

	//レイトレ実行。
	if (m_raytracingFlag)
	{
		//Tlasを構築 or 再構築する。
		m_tlas.Build(m_blasVector);

		//レイトレ用のデータを構築。
		m_rayPipeline->BuildShaderTable(m_blasVector);

		m_rayPipeline->TraceRay(m_tlas);
	}
	/*----- レイトレ描画終了 -----*/


	ImGui::Begin("Light");
	ImGui::DragFloat("VecX", &m_lightVec.x);
	ImGui::DragFloat("VecY", &m_lightVec.y);
	ImGui::DragFloat("VecZ", &m_lightVec.z);
	ImGui::DragFloat("AtemX", &m_atem.x);
	ImGui::DragFloat("AtemY", &m_atem.y);
	ImGui::DragFloat("AtemZ", &m_atem.z);
	for (auto& obj : m_drawPlaneArray)
	{
		ImGui::Checkbox(obj.m_bufferName.c_str(), &obj.m_drawFlag);
	}
	ImGui::Checkbox(m_drawFinalPlane.m_bufferName.c_str(), &m_drawFinalPlane.m_drawFlag);
	ImGui::Checkbox("RayTracing", &m_raytracingFlag);
	ImGui::Checkbox("DrawLightBox", &m_lightFlag);
	KazImGuiHelper::InputVec3("AlphaModel", &m_alphaModelTransform.pos);
	ImGui::End();

	//ディレクションライト
	ImGui::Begin("DirLight");
	ImGui::SliderFloat("VecX", &GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.x, -1.0f, 1.0f);
	ImGui::SliderFloat("VecY", &GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.y, -1.0f, 1.0f);
	ImGui::SliderFloat("VecZ", &GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.z, -1.0f, 1.0f);
	//GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.Normalize();
	bool isActive = GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_isActive;
	ImGui::Checkbox("ActiveFlag", &isActive);
	GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_isActive = isActive;
	ImGui::End();

	//ポイントライト
	ImGui::Begin("PointLight");
	ImGui::DragFloat("PosX", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_pos.x, 0.5f);
	ImGui::DragFloat("PosY", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_pos.y, 0.5f);
	ImGui::DragFloat("PosZ", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_pos.z, 0.5f);
	ImGui::DragFloat("Power", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_power, 0.5f, 1.0f);
	isActive = GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_isActive;
	ImGui::Checkbox("ActiveFlag", &isActive);
	GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_isActive = isActive;
	ImGui::End();

	//ボリュームフォグ
	ImGui::Begin("VolumeFog");
	ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);
	ImGui::DragFloat("WindSpeed", &m_noiseParam.m_windSpeed, 0.1f, 0.1f, 10.0f);
	//風の強度
	ImGui::DragFloat("WindStrength", &m_noiseParam.m_windStrength, 0.1f, 0.1f, 1.0f);
	//風のしきい値 ノイズを風として判断するためのもの
	ImGui::DragFloat("WindThreshold", &m_noiseParam.m_threshold, 0.01f, 0.01f, 1.0f);
	//ノイズのスケール
	ImGui::DragFloat("NoiseScale", &m_noiseParam.m_skydormScale, 1.0f, 1.0f, 2000.0f);
	//ノイズのオクターブ数
	ImGui::DragInt("NoiseOctaves", &m_noiseParam.m_octaves, 1, 1, 10);
	//ノイズの持続度 違う周波数のノイズを計算する際にどのくらいノイズを持続させるか。 粒度になる。
	ImGui::DragFloat("NoisePersistance", &m_noiseParam.m_persistence, 0.01f, 0.01f, 1.0f);
	//ノイズの黒っぽさ
	ImGui::DragFloat("NoiseLacunarity", &m_noiseParam.m_lacunarity, 0.01f, 0.01f, 10.0f);
	ImGui::Text(" ");
	//ボリュームテクスチャの座標
	std::array<float, 3> boxPos = { m_raymarchingParam.m_pos.x,m_raymarchingParam.m_pos.y, m_raymarchingParam.m_pos.z };
	ImGui::DragFloat3("Position", boxPos.data(), 0.1f);
	m_raymarchingParam.m_pos = KazMath::Vec3<float>(boxPos[0], boxPos[1], boxPos[2]);
	//フォグの色
	std::array<float, 3> fogColor = { m_raymarchingParam.m_color.x,m_raymarchingParam.m_color.y, m_raymarchingParam.m_color.z };
	ImGui::DragFloat3("FogColor", fogColor.data(), 0.001f, 0.001f, 1.0f);
	m_raymarchingParam.m_color = KazMath::Vec3<float>(fogColor[0], fogColor[1], fogColor[2]);
	ImGui::DragFloat("WrapCount", &m_raymarchingParam.m_wrapCount, 1.0f, 1.0f, 100.0f);
	ImGui::DragFloat("GridSize", &m_raymarchingParam.m_gridSize, 0.1f, 0.1f, 1000.0f);
	ImGui::DragFloat("SamplingLength", &m_raymarchingParam.m_sampleLength, 0.1f, 1.0f, 1000.0f);
	ImGui::DragFloat("Density", &m_raymarchingParam.m_density, 0.01f, 0.0f, 10.0f);
	ImGui::End();

	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//データを転送。一旦ここで。
	GBufferMgr::Instance()->m_lightBuffer.bufferWrapper->TransData(&GBufferMgr::Instance()->m_lightConstData, sizeof(GBufferMgr::LightConstData));
}

int RenderScene::SceneChange()
{
	return 0;
}
