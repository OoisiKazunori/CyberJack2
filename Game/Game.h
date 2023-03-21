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
#include"../Game/Enemy/HitEnemyEffect.h"
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


class Game
{

public:
	//定数--------------------------


	Game(const std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::LAYER_LEVEL_MAX> &RESPONE_DATA,
		const std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX> &STAGE_ARRAY,
		const std::array<KazMath::Color, KazEnemyHelper::STAGE_NUM_MAX> &BACKGROUND_COLOR,
		const std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX> &CAMERA_ARRAY);
	~Game();
	void Init(
		bool SKIP_FLAG
	);
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:

	const float LOG_FONT_SIZE;
	AttackLog stringLog;


	//ゲームループ----------------------------------------------------------------
	bool gameStartFlag;						//ゲーム開始を知らせるフラグ
	int sceneNum;							//次何処のシーンに飛ぶか
	bool sceneChangeFlag;
	int gameFlame;							//1ステージの経過時間
	int gameSpeed;							//1ステージで進む時間のスピード
	int gameStageLevel;						//現在のステージのレベル
	int gameLeyerLevel;						//現在のステージのレベル
	std::array<int, 10>changeLayerLevelMaxTime;	//次のレイヤーレベルに切り替わるフレーム数を保管する
	//ゲームループ----------------------------------------------------------------

	//デバック用-------------------------------------------------------------
	BackGroundForDebug bg;
	//デバック用-------------------------------------------------------------


	//カメラ----------------------------------------------------------------
	KazMath::Vec3<float> eyePos;
	KazMath::Vec3<float> targetPos;
	std::array<std::array<KazEnemyHelper::ForceCameraData, 10>, KazEnemyHelper::STAGE_NUM_MAX>cameraMoveArray;
	float cameraAngle;
	CameraWork cameraWork;
	//カメラ----------------------------------------------------------------

	//プレイヤーが操作するもの----------------------------------------------------------------
	Player player;
	Cursor cursor;
	//プレイヤーが操作するもの----------------------------------------------------------------

	//敵----------------------------------------------------------------
	std::array<unique_ptr<IEnemy>, 2>enemy;					//敵(サンプル)
	std::array<std::array<unique_ptr<IEnemy>, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::LAYER_LEVEL_MAX> enemies;	//1ステージに生成する敵の総数
	std::array<int, 10> enemiesHandle;						//0から順番に初期化する際に必要
	std::array<int, 10> addEnemiesHandle;					//0から順番に追加で初期化する際に必要
	std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::LAYER_LEVEL_MAX> responeData;		//敵を生成する際に必要な設定
	std::array<ResponeData, 50>addResponeData;				//敵を追加で生成する際に必要な設定をスタックしたもの
	bool isEnemyNotMoveFlag;
	int notMoveTimer;
	const int CHANGE_GMAE_FLAME_SPEED_MAX_TIME = 2;	//敵が居なくなってからゲーム内時間の進むスピードが速まるまでの間隔

#ifdef _DEBUG
	std::array<std::array<BoxPolygonRender, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::LAYER_LEVEL_MAX> enemyHitBox;
#endif
	//敵----------------------------------------------------------------

	//線演出----------------------------------------------------------------
	static const int LINE_MAX_ARRAY_NUM = 16;
	std::array<LineLevel1, LINE_MAX_ARRAY_NUM> lineLevel;			//直角に敵に線を伸ばしていく演出
	std::array<LineEffectData, LINE_MAX_ARRAY_NUM>lineEffectArrayData;
	KazMath::Vec3<float> testEnemyPos;			//ロックオンする対象座標(テスト)
	//線演出----------------------------------------------------------------

	//攻撃演出-------------------------
	std::array<HitEnemyEffect, 30> hitEffect;
	//攻撃演出-------------------------

	ObjModelRenderPtr model;

	GoalBox goalBox;
	std::array<KazMath::Vec3<float>, KazEnemyHelper::STAGE_NUM_MAX> appearGoalBoxPos;
	KazMath::Vec3<float> responeGoalBoxPos;
	bool initAppearFlag;
	bool changeStageFlag;

	//画面効果準備-----------------------

	Sprite2DRender mainRenderTarget;
	std::array<std::unique_ptr<GameRenderTarget>, KazEnemyHelper::STAGE_NUM_MAX> renderTarget;
	std::array<std::unique_ptr<GameRenderTarget>, KazEnemyHelper::STAGE_NUM_MAX> nextRenderTarget;

	int stageNum;
	std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>stages;
	bool cameraModeChangeFlag;
	bool lineDebugFlag;



	std::array<BoxPolygonRender, 8> lineStartPoly;


	//ゲームオーバー画面----------------------------------------------
	Sprite2DRender gameOverTex;
	Sprite2DRender pressAButtonTex;
	Sprite2DRender blackTex;
	bool gameOverFlag;
	bool readyToBlackOutFlag;
	bool readyToBlackOutToGoTitleFlag;
	int flashTimer;
	bool flashFlag;
	bool isGameOverFlag;

	RESOURCE_HANDLE gameClearResourceHandle, gameOverResouceHandle;
	//ゲームオーバー画面----------------------------------------------


	//ゲームクリア画面--------------------------------------------
	bool isGameClearFlag;
	bool gameClearFlag;
	//ゲームクリア画面--------------------------------------------

	//UI--------------------------------------
	AnnounceStageUI stageUI;


	RESOURCE_HANDLE bgmSoundHandle, lockSoundHandle;
	RESOURCE_HANDLE doneSoundHandle;
	RESOURCE_HANDLE damageSoundHandle;

	TutorialWindow tutorialWindow;

	KazMath::Vec3<float>tPos;

	CircleRender circle;

	bool smokeFlag;
	std::array<Sprite2DRender, 4> smokeR;

	int emittNum;

	//エフェクト--------------------------------------
	std::array<FireEffect, 30>fireEffect;
	std::array<RocketLightEffect, 30>lightEffect;
	int rocketIndex;
	int fireIndex;

	//エフェクト--------------------------------------

	BoxPolygonRender box;

	std::unique_ptr<Tutorial> tutorial;

	PortalIntermediateDirection portalEffect;
	//std::unique_ptr<MeshParticleEmitter> meshEmitter;


	FbxModelRender playerModel, headModel;


	bool d;
	std::unique_ptr<StringWindow> logoutWindow;
	bool prepareToClearFlag;
	bool initEndLogFlag;
	int gameClearTimer;

	KazMath::Vec3<float>p1;
	Sphere t1;
	std::vector<Sphere *>enemyHitBoxArray;
	std::unique_ptr<InstanceMeshCollision> meshCollision;
	std::unique_ptr<InstanceMeshParticle>meshParticle;
	std::unique_ptr<GPUParticleRender>particleRender;
};
