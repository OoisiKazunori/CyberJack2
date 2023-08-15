#include "SceneManager.h"
#include"../Helper/KazHelper.h"
#include"../Sound/SoundManager.h"
#include"../Sound/SoundManager.h"
#include"../Helper/ResourceFilePass.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Fps/FPSManager.h"
#include"../Scene/PortalScene.h"
#include"../Scene/EnemyDebugScene.h"
#include"../Scene/DebugStageScene.h"
#include"../Scene/DebugMeshParticle.h"
#include"../Scene/RayTracingScene.h"
#include"../Scene/RenderScene.h"
#include"../Scene/GameScene.h"
#include"../Scene/GameClearScene.h"
#include"../Scene/GameOverScene.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

SceneManager::SceneManager() :gameFirstInitFlag(false)
{
	scene.emplace_back(std::make_unique<GameScene>());
	scene.emplace_back(std::make_unique<TitleScene>());
	scene.emplace_back(std::make_unique<GameClearScene>());
	scene.emplace_back(std::make_unique<GameOverScene>());

	nowScene = 0;
	nextScene = 0;
	itisInArrayFlag = true;
	endGameFlag = false;
	initGameFlag = false;
	m_raytracingFlag = true;

	change = std::make_unique<ChangeScene::SceneChange>();


	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS", L"checkHitRayMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 7;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 6, 5, 4, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);


	m_debugOnOffLineRender = DrawFuncData::SetTexPlaneData(DrawFuncData::GetSpriteShader());
	m_debugOnOffLineBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/UI/DebugOnOffLine.png");
	m_debugOnOffLineStayBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/UI/DebugOnOffLineStay.png");

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
	m_raymarchingParam.m_pos = KazMath::Vec3<float>(-100.0f, 1.0f, 1.0f);
	m_raymarchingParam.m_color = KazMath::Vec3<float>(1.0f, 1.0f, 1.0f);
	m_raymarchingParam.m_wrapCount = 20.0f;
	m_raymarchingParam.m_gridSize = 4.0f;
	m_raymarchingParam.m_wrapCount = 30.0f;
	m_raymarchingParam.m_density = 0.65f;
	//m_raymarchingParam.m_density = 1.0f;
	m_raymarchingParam.m_sampleLength = 8.0f;
	m_raymarchingParam.m_isSimpleFog = 0;
	m_raymarchingParam.m_isActive = false;
	m_raymarchingParamData = KazBufferHelper::SetConstBufferData(sizeof(RaymarchingParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//レイマーチングのパラメーター用定数バッファをセット。
	m_rayPipeline->SetRaymarchingConstData(&m_raymarchingParamData);

	//OnOffデバッグ用のパラメーターを用意。
	m_debugRaytracingParam.m_debugReflection = 0;
	m_debugRaytracingParam.m_debugShadow = 0;
	m_debugRaytracingParam.m_sliderRate = 1280.0f / 2.0f;
	m_debugRaytracingParamData = KazBufferHelper::SetConstBufferData(sizeof(DebugRaytracingParam));
	m_debugRaytracingParamData.bufferWrapper->TransData(&m_debugRaytracingParam, sizeof(DebugRaytracingParam));
	m_isDebugRaytracing = false;
	m_isOldDebugRaytracing = false;
	m_isDebugCamera = false;
	m_isDebugTimeZone = false;
	m_isDebugVolumeFog = false;
	m_isDebugSea = false;
	m_isPause = false;
	m_isMoveOnly1F = false;
	m_isSeaEffect = false;
	m_seaID = SEA_ID::CALM;

	m_debugTimeZone = 0;

	//OnOffデバッグ用のパラメーターを用意。
	m_rayPipeline->SetDebugOnOffConstData(&m_debugRaytracingParamData);


	m_debugSeaParam.m_freq = 0.16f;
	m_debugSeaParam.m_amp = 0.6f;
	m_debugSeaParam.m_choppy = 4.0f;
	m_debugSeaParam.m_seaSpeed = 5.8f;
	m_debugSeaParamData = KazBufferHelper::SetConstBufferData(sizeof(DebugSeaParam));
	m_debugSeaParamData.bufferWrapper->TransData(&m_debugSeaParam, sizeof(DebugSeaParam));
	m_rayPipeline->SetDebugSeaConstData(&m_debugSeaParamData);
}

SceneManager::~SceneManager()
{
	SoundManager::Instance()->Finalize();
}

void SceneManager::Update()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	if (m_isPause && !m_isMoveOnly1F) {
		m_blasVector.Update();
		return;
	}

	//シーン遷移の開始
	if (nextScene != nowScene)
	{
		change->Start();
	}

	if (!gameFirstInitFlag)
	{
		scene[nowScene]->Init();
		gameFirstInitFlag = true;
		initGameFlag = false;
	}


	const int RESTART_NUM = -2;

	//画面が完全に隠れてから1F分ずらす
	if (initGameFlag)
	{
		if (KazHelper::IsitInAnArray(nowScene, scene.size()))
		{
			scene[nowScene]->Init();
			itisInArrayFlag = true;
		}
		else
		{
			itisInArrayFlag = false;
		}
		initGameFlag = false;
	}

	//ゲーム画面が隠された判定
	if (change->AllHiden())
	{
		scene[nowScene]->Finalize();

		if (nextScene != RESTART_NUM)
		{
			nowScene = nextScene;
		}
		else if (nextScene == RESTART_NUM)
		{
			nextScene = nowScene;
		}

		if (!scene[nextScene]->firstGenerateFlag)
		{
			scene[nextScene]->PreInit();
		}
		scene[nextScene]->firstGenerateFlag = false;
		initGameFlag = true;
	}

	//更新処理
	if (itisInArrayFlag && !initGameFlag)
	{
		scene[nowScene]->Input();
		scene[nowScene]->Update();

		int sceneNum = scene[nowScene]->SceneChange();
		if (sceneNum != SCENE_NONE)
		{
			nextScene = sceneNum;
		}

		if (scene[nowScene]->endGameFlag)
		{
			endGameFlag = true;
		}
	}

	change->Update();

	m_blasVector.Update();

	//fpsを制限(今回は60fps)
	FpsManager::RegulateFps(60);


	//ボリュームノイズを書き込む。
	m_noiseParam.m_timer += 0.001f;
	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	//m_volumeNoiseShader.Compute({ static_cast<UINT>(256 / 8), static_cast<UINT>(256 / 8), static_cast<UINT>(256 / 4) });

	//ノイズ用のタイマーを加算。
	GBufferMgr::Instance()->m_cameraEyePosData.m_noiseTimer += 0.02f;

	//デバッグ実行中はOnOffのラインをつかんで動かせるようにする。
	if (m_isDebugRaytracing) {

		//左クリックしていたら。
		bool isMouseLeftClick = KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_LEFT);
		if (isMouseLeftClick) {

			//マウスの移動量を保存して動かす。
			m_debugRaytracingParam.m_sliderRate = KeyBoradInputManager::Instance()->GetMousePoint().x;
			m_debugRaytracingParam.m_sliderRate = std::clamp(m_debugRaytracingParam.m_sliderRate, 0.0f, 1280.0f);

		}

	}

	m_isOldDebugRaytracing = m_isDebugRaytracing;

}

void SceneManager::Draw()
{
	change->Draw(m_rasterize);

	if (itisInArrayFlag)
	{
		scene[nowScene]->Draw(m_rasterize, m_blasVector);
	}

	//デバッグ用のOnOffのラインを描画する。
	if (m_isDebugRaytracing) {
		m_debugOnOffLineTransform.pos.x = m_debugRaytracingParam.m_sliderRate;
		m_debugOnOffLineTransform.pos.y = 720.0f / 2.0f;
		m_debugOnOffLineTransform.scale.x = 10.0f;
		m_debugOnOffLineTransform.scale.y = 720.0f;
		if (KeyBoradInputManager::Instance()->MouseInputState(MOUSE_INPUT_LEFT)) {
			DrawFunc::DrawTextureIn2D(m_debugOnOffLineRender, m_debugOnOffLineTransform, m_debugOnOffLineBuffer);
		}
		else {
			DrawFunc::DrawTextureIn2D(m_debugOnOffLineRender, m_debugOnOffLineTransform, m_debugOnOffLineStayBuffer);
		}
		m_rasterize.ObjectRender(m_debugOnOffLineRender);
	}

	m_rasterize.Sort();
	m_rasterize.Render();

	if (m_raytracingFlag)
	{
		//Tlasを構築 or 再構築する。
		m_tlas.Build(m_blasVector);
		//レイトレ用のデータを構築。
		m_rayPipeline->BuildShaderTable(m_blasVector);
		if (m_blasVector.GetBlasRefCount() != 0)
		{
			m_rayPipeline->TraceRay(m_tlas);
		}
	}

	//デバッグメニューの大本
	ImGui::Begin("DebugMenu");

	ImGui::Checkbox("DebugCamera", &m_isDebugCamera);
	ImGui::Checkbox("Raytracing", &m_isDebugRaytracing);
	ImGui::Checkbox("TimeZone", &m_isDebugTimeZone);
	ImGui::Checkbox("VolumeFog", &m_isDebugVolumeFog);
	ImGui::Checkbox("Sea", &m_isDebugSea);

	ImGui::End();

	//カメラのデバッグメニュー
	if (m_isDebugCamera) {

		m_isPause = true;

		ImGui::Begin("DebugCamera");

		m_isMoveOnly1F = ImGui::Button("MoveOnly1F");

		ImGui::End();

	}
	else {
		m_isPause = false;
	}

	//レイトレのデバッグメニュー
	if (m_isDebugRaytracing) {

		ImGui::Begin("Raytracing");

		bool checkBox = m_debugRaytracingParam.m_debugReflection;
		ImGui::Checkbox("REFLECT", &checkBox);
		m_debugRaytracingParam.m_debugReflection = checkBox;
		ImGui::SameLine();
		checkBox = m_debugRaytracingParam.m_debugShadow;
		ImGui::Checkbox("SHADOW", &checkBox);
		m_debugRaytracingParam.m_debugShadow = checkBox;
		ImGui::SliderFloat("RATE", &m_debugRaytracingParam.m_sliderRate, 0.0f, 1280.0f);

		ImGui::End();

	}
	else {
		m_debugRaytracingParam.m_debugReflection = false;
		m_debugRaytracingParam.m_debugShadow = false;
		m_debugRaytracingParam.m_sliderRate = 1280.0f / 2.0f;
	}
	//このデバッグ機能が切り替わった瞬間だったら初期値を入れる。
	if (m_isDebugRaytracing && !m_isOldDebugRaytracing) {
		m_debugRaytracingParam.m_debugReflection = true;
		m_debugRaytracingParam.m_debugShadow = true;
		m_debugRaytracingParam.m_sliderRate = 1280.0f / 2.0f;
	}
	m_debugRaytracingParamData.bufferWrapper->TransData(&m_debugRaytracingParam, sizeof(DebugRaytracingParam));

	//時間帯のデバッグメニュー
	if (m_isDebugTimeZone) {

		ImGui::Begin("TimeZone");

		ImGui::RadioButton("Noon", &m_debugTimeZone, 0);
		ImGui::RadioButton("Evening", &m_debugTimeZone, 1);

		ImGui::End();

		//選択されている値によってDirLightの角度を変える。
		if (m_debugTimeZone == 0) {
			GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir += (KazMath::Vec3<float>(0.0f, -0.857f, 0.514f) - GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir) / 10.0f;
		}
		else if (m_debugTimeZone == 1) {
			GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir += (KazMath::Vec3<float>(0.0f, -0.683f, -0.73f) - GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir) / 10.0f;
		}
		GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.Normalize();

	}

	//ボリュームフォグのデバッグメニュー
	if (m_isDebugVolumeFog) {

		ImGui::Begin("VolumeFog");

		ImGui::End();

	}

	//海のデバッグメニュー
	if (m_isDebugSea) {

		ImGui::Begin("Sea");

		const float CALM_SEA_AMP = 0.02f;
		const float CALM_SEA_FREQ = 0.16f;

		const float NORMAL_SEA_AMP = 0.6f;
		const float NORMAL_SEA_FREQ = 0.16f;

		const float STORMY_SEA_AMP = 2.8f;
		const float STORMY_SEA_FREQ = 0.16f;

		const float EFFECT_FREQ = 0.1f;

		const float SEA_SPEED = 5.8f;

		ImGui::RadioButton("Calm", &m_seaID, 0);
		ImGui::SameLine();
		ImGui::RadioButton("Normal", &m_seaID, 1);
		ImGui::SameLine();
		ImGui::RadioButton("Stormy", &m_seaID, 2);

		ImGui::Checkbox("IsEffect", &m_isSeaEffect);

		//静かな海だったら
		float baseAmp = 0.0f;
		float baseFreq = 0.0f;
		float baseSeaSpeed = SEA_SPEED;
		if (m_seaID == SEA_ID::CALM) {
			baseAmp = CALM_SEA_AMP;
			baseFreq = CALM_SEA_FREQ;
		}
		else if (m_seaID == SEA_ID::NORMAL) {
			baseAmp = NORMAL_SEA_AMP;
			baseFreq = NORMAL_SEA_FREQ;
		}
		else if (m_seaID == SEA_ID::STORMY) {
			baseAmp = STORMY_SEA_AMP;
			baseFreq = STORMY_SEA_FREQ;
		}
		if (m_isSeaEffect) {
			baseAmp += 2.0f;
			baseFreq += EFFECT_FREQ;
			baseSeaSpeed += 30.0f;
		}
		m_debugSeaParam.m_amp += (baseAmp - m_debugSeaParam.m_amp) / 3.0f;
		m_debugSeaParam.m_freq += (baseFreq - m_debugSeaParam.m_freq) / 3.0f;
		m_debugSeaParam.m_seaSpeed += (baseSeaSpeed - m_debugSeaParam.m_seaSpeed) / 10.0f;

		ImGui::End();

		m_debugSeaParamData.bufferWrapper->TransData(&m_debugSeaParam, sizeof(DebugSeaParam));

	}


	////ディレクションライト
	//ImGui::Begin("DirLight");
	//ImGui::SliderFloat("VecX", &GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.x, -1.0f, 1.0f);
	//ImGui::SliderFloat("VecY", &GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.y, -1.0f, 1.0f);
	//ImGui::SliderFloat("VecZ", &GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.z, -1.0f, 1.0f);
	//GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.Normalize();
	//bool isActive = GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_isActive;
	//ImGui::Checkbox("ActiveFlag", &isActive);
	//GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_isActive = isActive;
	//ImGui::End();

	////ポイントライト
	//ImGui::Begin("PointLight");
	//ImGui::DragFloat("PosX", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_pos.x, 0.5f);
	//ImGui::DragFloat("PosY", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_pos.y, 0.5f);
	//ImGui::DragFloat("PosZ", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_pos.z, 0.5f);
	//ImGui::DragFloat("Power", &GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_power, 0.5f, 1.0f);
	//isActive = GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_isActive;
	//ImGui::Checkbox("ActiveFlag", &isActive);
	//GBufferMgr::Instance()->m_lightConstData.m_pointLight.m_isActive = isActive;
	//ImGui::End();

	////OnOff
	//ImGui::Begin("DebugOnOff");
	//ImGui::Checkbox("IsDebug", &m_isDebugOnOff);
	//if (m_isDebugOnOff) {
	//	bool checkBox = m_onOffDebugParam.m_debugReflection;
	//	ImGui::Checkbox("REFLECT", &checkBox);
	//	m_onOffDebugParam.m_debugReflection = checkBox;
	//	ImGui::SameLine();
	//	checkBox = m_onOffDebugParam.m_debugShadow;
	//	ImGui::Checkbox("SHADOW", &checkBox);
	//	m_onOffDebugParam.m_debugShadow = checkBox;
	//	ImGui::SliderFloat("RATE", &m_onOffDebugParam.m_sliderRate, 0.0f, 1280.0f);
	//}
	//else {
	//	m_onOffDebugParam.m_debugReflection = false;
	//	m_onOffDebugParam.m_debugShadow = false;
	//	m_onOffDebugParam.m_sliderRate = 1280.0f / 2.0f;
	//}
	//ImGui::End();

	////このデバッグ機能が切り替わった瞬間だったら初期値を入れる。
	//if (m_isDebugOnOff && !m_isOldDebugOnOff) {
	//	m_onOffDebugParam.m_debugReflection = true;
	//	m_onOffDebugParam.m_debugShadow = true;
	//	m_onOffDebugParam.m_sliderRate = 1280.0f / 2.0f;
	//}
	//m_OnOffDebugParamData.bufferWrapper->TransData(&m_onOffDebugParam, sizeof(OnOffDebugParam));

	////ボリュームフォグ
	//ImGui::Begin("VolumeFog");
	//ImGui::SetWindowSize(ImVec2(400, 100), ImGuiCond_::ImGuiCond_FirstUseEver);
	//ImGui::DragFloat("WindSpeed", &m_noiseParam.m_windSpeed, 0.1f, 0.1f, 10.0f);
	////風の強度
	//ImGui::DragFloat("WindStrength", &m_noiseParam.m_windStrength, 0.1f, 0.1f, 1.0f);
	////風のしきい値 ノイズを風として判断するためのもの
	//ImGui::DragFloat("WindThreshold", &m_noiseParam.m_threshold, 0.01f, 0.01f, 1.0f);
	////ノイズのスケール
	//ImGui::DragFloat("NoiseScale", &m_noiseParam.m_skydormScale, 1.0f, 1.0f, 2000.0f);
	////ノイズのオクターブ数
	//ImGui::DragInt("NoiseOctaves", &m_noiseParam.m_octaves, 1, 1, 10);
	////ノイズの持続度 違う周波数のノイズを計算する際にどのくらいノイズを持続させるか。 粒度になる。
	//ImGui::DragFloat("NoisePersistance", &m_noiseParam.m_persistence, 0.01f, 0.01f, 1.0f);
	////ノイズの黒っぽさ
	//ImGui::DragFloat("NoiseLacunarity", &m_noiseParam.m_lacunarity, 0.01f, 0.01f, 10.0f);
	//ImGui::Text(" ");
	////ボリュームテクスチャの座標
	//std::array<float, 3> boxPos = { m_raymarchingParam.m_pos.x,m_raymarchingParam.m_pos.y, m_raymarchingParam.m_pos.z };
	//ImGui::DragFloat3("Position", boxPos.data(), 0.1f);
	//m_raymarchingParam.m_pos = KazMath::Vec3<float>(boxPos[0], boxPos[1], boxPos[2]);
	////フォグの色
	//std::array<float, 3> fogColor = { m_raymarchingParam.m_color.x,m_raymarchingParam.m_color.y, m_raymarchingParam.m_color.z };
	//ImGui::DragFloat3("FogColor", fogColor.data(), 0.001f, 0.001f, 1.0f);
	//m_raymarchingParam.m_color = KazMath::Vec3<float>(fogColor[0], fogColor[1], fogColor[2]);
	//ImGui::DragFloat("WrapCount", &m_raymarchingParam.m_wrapCount, 1.0f, 1.0f, 100.0f);
	//ImGui::DragFloat("GridSize", &m_raymarchingParam.m_gridSize, 0.1f, 0.1f, 1000.0f);
	//ImGui::DragFloat("SamplingLength", &m_raymarchingParam.m_sampleLength, 0.1f, 1.0f, 1000.0f);
	//ImGui::DragFloat("Density", &m_raymarchingParam.m_density, 0.01f, 0.0f, 10.0f);
	//ImGui::SliderInt("IsActive", &m_raymarchingParam.m_isActive, 0, 1);
	//ImGui::End();

	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//データを転送。一旦ここで。
	GBufferMgr::Instance()->m_lightBuffer.bufferWrapper->TransData(&GBufferMgr::Instance()->m_lightConstData, sizeof(GBufferMgr::LightConstData));

}
