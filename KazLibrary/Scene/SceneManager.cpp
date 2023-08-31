#include "SceneManager.h"
#include"../Helper/KazHelper.h"
#include"../Sound/SoundManager.h"
#include"../Sound/SoundManager.h"
#include"../Helper/ResourceFilePass.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Fps/FPSManager.h"
#include"../Scene/RayTracingScene.h"
#include"../Scene/RenderScene.h"
#include"../Scene/GameScene.h"
#include"../Game/Effect/SeaEffect.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../Game/Effect/ShakeMgr.h"
#include"../Scene/ModelToolScene.h"
#include"../../Game/Effect/TimeZone.h"
#include"../Game/Effect/ShockWave.h"
#include"../../Game/Effect/StopMgr.h"
#include"../Game/Effect/EnemyDissolveParam.h"
#include"../Game/UI/OptionUI.h"
#include <Input/ControllerInputManager.h>
#include"../KazLibrary/Sound/SoundManager.h"

SceneManager::SceneManager() :gameFirstInitFlag(false)
{

	//デモ用のゲームシーンを設定。
	scene.emplace_back(std::make_unique<GameScene>());

	//シーン遷移を設定
	change = std::make_unique<ChangeScene::SceneChange>();

	//シーン番号、遷移に関するパラメーターを設定。
	nowScene = 0;
	nextScene = 0;
	itisInArrayFlag = true;
	endGameFlag = false;
	initGameFlag = false;

	//レイトレーシング周りを設定
	Raytracing::HitGroupMgr::Instance()->Setting();
	m_pipelineShaders.push_back({ "Resource/ShaderFiles/RayTracing/RaytracingShader.hlsl", {L"mainRayGen"}, {L"mainMS", L"shadowMS", L"checkHitRayMS"}, {L"mainCHS", L"mainAnyHit"} });
	int payloadSize = sizeof(float) * 7;
	m_rayPipeline = std::make_unique<Raytracing::RayPipeline>(m_pipelineShaders, Raytracing::HitGroupMgr::DEF, 6, 5, 4, payloadSize, static_cast<int>(sizeof(KazMath::Vec2<float>)), 6);


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
	TimeZone::Instance()->m_isSkyEffect = false;
	SeaEffect::Instance()->m_isSeaEffect = false;

	//ヒットストップのタイマーを初期化。
	StopMgr::Instance()->Init();

	TimeZone::Instance()->m_timeZone = 0;
	m_debugRaytracingParam.m_skyFacter = 1.0f;

	//OnOffデバッグ用のパラメーターを用意。
	m_rayPipeline->SetDebugOnOffConstData(&m_debugRaytracingParamData);

	SeaEffect::Instance()->Setting();
	m_rayPipeline->SetDebugSeaConstData(&SeaEffect::Instance()->m_debugSeaParamData);

	ShockWave::Instance()->Setting();

	OptionUI::Instance()->Setting();

	EnemyDissolveParam::Instance()->Setting();

	SoundManager::Instance()->SettingSoundManager();

	m_debugLineScale = 0;

	m_BGN = SoundManager::Instance()->SoundLoadWave("Resource/Sound/bgm.wav");
	m_BGN.volume = 0.005f;
	SoundManager::Instance()->SoundPlayerWave(m_BGN, 100);
}

SceneManager::~SceneManager()
{
}

void SceneManager::Update()
{
	DescriptorHeapMgr::Instance()->SetDescriptorHeap();

	if (StopMgr::Instance()->IsHitStop()) {
		StopMgr::Instance()->Update();
		m_blasVector.Update();
		return;
	}

	//シェイク量を更新。
	ShakeMgr::Instance()->Update();

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

	SeaEffect::Instance()->m_isOldSeaEffect = SeaEffect::Instance()->m_isSeaEffect;
	SeaEffect::Instance()->m_isSeaEffect = false;

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
	if (OptionUI::Instance()->m_isRaytracingDebug) {

		//左スティックでも動かせる。
		const float STICK_SPEED = 10.0f;
		bool rightFLag =
			ControllerInputManager::Instance()->InputStickState(ControllerStickSide::RIGHT_STICK, ControllerSide::RIGHT_SIDE) ||
			ControllerInputManager::Instance()->InputShoulderState(ControllerShoulderSide::RIGHT_SHOULDER) ||
			ControllerInputManager::Instance()->InputState(XINPUT_GAMEPAD_DPAD_RIGHT) ||
			ControllerInputManager::Instance()->InputState(XINPUT_GAMEPAD_RIGHT_THUMB) ||
			KeyBoradInputManager::Instance()->InputState(DIK_E);
		bool leftFLag =
			ControllerInputManager::Instance()->InputStickState(ControllerStickSide::RIGHT_STICK, ControllerSide::LEFT_SIDE) ||
			ControllerInputManager::Instance()->InputShoulderState(ControllerShoulderSide::LEFT_SHOULDER) ||
			ControllerInputManager::Instance()->InputState(XINPUT_GAMEPAD_DPAD_LEFT) ||
			ControllerInputManager::Instance()->InputState(XINPUT_GAMEPAD_LEFT_THUMB) ||
			KeyBoradInputManager::Instance()->InputState(DIK_Q);

		m_debugRaytracingParam.m_sliderRate += rightFLag * STICK_SPEED;
		m_debugRaytracingParam.m_sliderRate -= leftFLag * STICK_SPEED;
		m_debugRaytracingParam.m_sliderRate = std::clamp(m_debugRaytracingParam.m_sliderRate, 0.0f, 1280.0f);

		m_debugRaytracingParam.m_debugReflection = true;
		m_debugRaytracingParam.m_debugShadow = true;

	}
	else {
		m_debugRaytracingParam.m_debugReflection = false;
		m_debugRaytracingParam.m_debugShadow = false;
		m_debugRaytracingParam.m_sliderRate = 1280.0f / 2.0f;

	}

	TimeZone::Instance()->Update();

	ShockWave::Instance()->Update();

	EnemyDissolveParam::Instance()->Update();

	m_isOldDebugRaytracing = OptionUI::Instance()->m_isRaytracingDebug;

	OptionUI::Instance()->Update();


	//デバッグ用の値の更新処理
	m_debugRaytracingParamData.bufferWrapper->TransData(&m_debugRaytracingParam, sizeof(DebugRaytracingParam));

	//選択されている値によってDirLightの角度を変える。
	if (TimeZone::Instance()->m_timeZone == 0) {
		GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir += (KazMath::Vec3<float>(0.0f, -0.857f, 0.514f) - GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir) / 30.0f;
	}
	else if (TimeZone::Instance()->m_timeZone == 1) {
		GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir += (KazMath::Vec3<float>(0.0f, -0.683f, -0.73f) - GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir) / 30.0f;
	}
	GBufferMgr::Instance()->m_lightConstData.m_dirLight.m_dir.Normalize();

	if (TimeZone::Instance()->m_isSkyEffect) {
		m_debugRaytracingParam.m_skyFacter += (0.25f - m_debugRaytracingParam.m_skyFacter) / 5.0f;
	}
	else {
		m_debugRaytracingParam.m_skyFacter += (1.0f - m_debugRaytracingParam.m_skyFacter) / 5.0f;
	}

	SeaEffect::Instance()->Update();

	m_noiseParamData.bufferWrapper->TransData(&m_noiseParam, sizeof(NoiseParam));
	m_raymarchingParamData.bufferWrapper->TransData(&m_raymarchingParam, sizeof(RaymarchingParam));

	//データを転送。一旦ここで。
	GBufferMgr::Instance()->m_lightBuffer.bufferWrapper->TransData(&GBufferMgr::Instance()->m_lightConstData, sizeof(GBufferMgr::LightConstData));

}

void SceneManager::Draw()
{
	change->Draw(m_rasterize);


	//UIを描画
	OptionUI::Instance()->Draw(m_rasterize, m_debugRaytracingParam.m_sliderRate);
	if (itisInArrayFlag)
	{
		scene[nowScene]->Draw(m_rasterize, m_blasVector);
	}

	m_rasterize.Sort();
	m_rasterize.Render();

	//Tlasを構築 or 再構築する。
	m_tlas.Build(m_blasVector);
	//レイトレ用のデータを構築。
	m_rayPipeline->BuildShaderTable(m_blasVector);
	if (m_blasVector.GetBlasRefCount() != 0)
	{
		m_rayPipeline->TraceRay(m_tlas);
	}

}
