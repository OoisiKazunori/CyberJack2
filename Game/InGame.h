#pragma once
#include"../KazLibrary/Render/BackGroundForDebug.h"
#include"../KazLibrary/Render/KazRender.h"
#include<memory>
#include"../Game/Player.h"
#include"../Game/UI/Cursor.h"
#include"../Game/LineEffect/LineLevel1.h"
#include"../Game/Event/GoalBox.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/RenderTarget/GaussianBuler.h"
#include"../KazLibrary/Render/PolygonRender.h"
#include"../KazLibrary/Scene/SceneBase.h"
#include"../Game/Interface/IStage.h"
#include"../Game/Stage/FirstStage.h"
#include"../Game/Stage/SecondStage.h"
#include"../Game/Stage/ThridStage.h"
#include"../Game/Stage/RezStage.h"
#include"../Game/Stage/BlockParticleStage.h"
#include"../Game/Stage/GpuParticleStage.h"

#include"../Game/UI/AnnounceStageUI.h"
#include"../Game/Event/EventSprite.h"
#include"../Game/Event/TutorialWindow.h"
#include"../Game/Event/PortalEffect.h"
#include"../Game/Effect/IHitEffectEmitter.h"
#include"../Game/Effect/HitEffectPattern1Emitter.h"
#include"../Game/Effect/HitEffectPattern2Emitter.h"
#include"../Game/Effect/HitEffectPattern3Emitter.h"
#include"../Game/Helper/EnemyHelper.h"

#include"../Game/Effect/FireEffect.h"
#include"../Game/Effect/RocketLightEffect.h"
#include"../Game/Effect/MeshParticleEmitter.h"
#include"../Game/Effect/DeadParticle.h"

#include"../Game/UI/AttackLog.h"
#include"../Game/Helper/CameraWork.h"

#include"Tutorial.h"
#include"../Game/UI/StringWindow.h"

#include"../Game/CollisionDetection/InstanceMeshCollision.h"
#include"../Game/Effect/InstanceDeadParticle.h"
#include"../Game/Tool/SplineRail.h"
#include"../Game/Debug/DebugCamera.h"

#include"../KazLibrary/Buffer/DrawFuncData.h"

#include"Raytracing/BlasVector.h"
#include"Raytracing/Tlas.h"
#include"Raytracing/RayPipeline.h"
#include"../KazLibrary/Render/DrawFunc.h"
#include"../KazLibrary/Helper/Compute.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

#include"../Game/Effect/InstanceMeshParticle.h"
#include"../KazLibrary/Buffer/CreateMeshBuffer.h"
#include"../KazLibrary/Sound/SoundManager.h"

class InGame
{
public:

	InGame(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX>& RESPONE_DATA,
		const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>& STAGE_ARRAY,
		const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX>& BACKGROUND_COLOR,
		const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>& CAMERA_ARRAY);
	void Init(
		bool SKIP_FLAG
	);
	void Finalize();
	void Input();
	void Update();
	void Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec);

	int SceneChange();

	void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

	std::array<std::array<shared_ptr<IEnemy>, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX> GetEnemy() { return m_enemies; }

private:
	//描画---------------------------------------

	//ボリュームフォグ用3Dテクスチャ
	KazBufferHelper::BufferData m_volumeFogTextureBuffer;
	ComputeShader m_volumeNoiseShader;
	struct NoiseParam
	{
		KazMath::Vec3<float> m_worldPos;
		float m_timer;
		float m_windSpeed;
		float m_windStrength;
		float m_threshold;
		float m_skydormScale;
		int m_octaves;
		float m_persistence;
		float m_lacunarity;
		float m_pad;
	}m_noiseParam;
	KazBufferHelper::BufferData m_noiseParamData;

	//ボリュームフォグ用定数バッファ
	struct RaymarchingParam
	{
		KazMath::Vec3<float> m_pos; //ボリュームテクスチャのサイズ
		float m_gridSize; //サンプリングするグリッドのサイズ
		KazMath::Vec3<float> m_color; //フォグの色
		float m_wrapCount; //サンプリング座標がはみ出した際に何回までWrapするか
		float m_sampleLength; //サンプリング距離
		float m_density; //濃度係数
		int m_isSimpleFog;
		float m_pad;
	}m_raymarchingParam;
	KazBufferHelper::BufferData m_raymarchingParamData;

	//描画---------------------------------------


	//インゲーム---------------------------------------
	//進行にかかわるもの
	bool m_gameStartFlag;						//ゲーム開始を知らせるフラグ
	bool m_sceneChangeFlag;
	int m_gameFlame;							//1ステージの経過時間
	int m_gameSpeed;							//1ステージで進む時間のスピード
	int m_gameStageLevel;						//現在のステージのレベル
	int m_gameLeyerLevel;						//現在のステージのレベル
	int m_sceneNum;

	Player m_player;
	Cursor m_cursor;
	KazMath::Vec2<float>m_prevMouseVel;
	SplineRail m_rail;
	std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>m_stageArray;
	CameraWork m_camera;

	//蝶の敵を数回に一回出現させるようにするための変数
	int m_butterflyEnemyRespawnDelay;
	const int BUTTERFLY_ENEMY_RESPAWN_DELAY = 3;

	//敵----------------------------------------------------------------
	std::array<unique_ptr<IEnemy>, 2>enemy;																					//敵(サンプル)
	std::array<std::array<shared_ptr<IEnemy>, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX> m_enemies;	//1ステージに生成する敵の総数
	std::array<int, KazEnemyHelper::ENEMY_TYPE_MAX> enemiesHandle;															//0から順番に初期化する際に必要
	std::array<int, KazEnemyHelper::ENEMY_TYPE_MAX> addEnemiesHandle;														//0から順番に追加で初期化する際に必要
	std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX> m_responeData;		//敵を生成する際に必要な設定
	std::array<ResponeData, 50>m_addResponeData;																			//敵を追加で生成する際に必要な設定をスタックしたもの
	bool m_isEnemyNotMoveFlag;
	int m_notMoveTimer;
	const int CHANGE_GMAE_FLAME_SPEED_MAX_TIME = 2;	//敵が居なくなってからゲーム内時間の進むスピードが速まるまでの間隔
	std::vector<Sphere*>m_enemyHitBoxArray;

	//敵----------------------------------------------------------------

	//インゲーム---------------------------------------

	//デバック処理---------------------------------------

	//ゲーム開始時の表示
	//推奨
	DrawFuncData::DrawCallData m_guideRender;
	KazBufferHelper::BufferData m_infomationTex;
	int m_guideTimer;
	float m_guideAlphaTimer;

	//操作方法
	DrawFuncData::DrawCallData m_guideUI;
	KazBufferHelper::BufferData m_guideTex;


public:
	bool m_debugFlag;
private:
	DebugCamera m_debugCamera;
	//デバック処理---------------------------------------

	//ブルームテスト
	DrawFuncData::DrawCallData m_bloomModelRender;

	int m_lightIndex = 0;

	RESOURCE_HANDLE m_bgmHandle;

	SoundData m_lockonSE;
};