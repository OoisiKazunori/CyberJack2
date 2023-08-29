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
	//�`��---------------------------------------

	//�{�����[���t�H�O�p3D�e�N�X�`��
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

	//�{�����[���t�H�O�p�萔�o�b�t�@
	struct RaymarchingParam
	{
		KazMath::Vec3<float> m_pos; //�{�����[���e�N�X�`���̃T�C�Y
		float m_gridSize; //�T���v�����O����O���b�h�̃T�C�Y
		KazMath::Vec3<float> m_color; //�t�H�O�̐F
		float m_wrapCount; //�T���v�����O���W���͂ݏo�����ۂɉ���܂�Wrap���邩
		float m_sampleLength; //�T���v�����O����
		float m_density; //�Z�x�W��
		int m_isSimpleFog;
		float m_pad;
	}m_raymarchingParam;
	KazBufferHelper::BufferData m_raymarchingParamData;

	//�`��---------------------------------------


	//�C���Q�[��---------------------------------------
	//�i�s�ɂ���������
	bool m_gameStartFlag;						//�Q�[���J�n��m�点��t���O
	bool m_sceneChangeFlag;
	int m_gameFlame;							//1�X�e�[�W�̌o�ߎ���
	int m_gameSpeed;							//1�X�e�[�W�Ői�ގ��Ԃ̃X�s�[�h
	int m_gameStageLevel;						//���݂̃X�e�[�W�̃��x��
	int m_gameLeyerLevel;						//���݂̃X�e�[�W�̃��x��
	int m_sceneNum;

	Player m_player;
	Cursor m_cursor;
	KazMath::Vec2<float>m_prevMouseVel;
	SplineRail m_rail;
	std::array<std::shared_ptr<IStage>, KazEnemyHelper::STAGE_NUM_MAX>m_stageArray;
	CameraWork m_camera;

	//���̓G�𐔉�Ɉ��o��������悤�ɂ��邽�߂̕ϐ�
	int m_butterflyEnemyRespawnDelay;
	const int BUTTERFLY_ENEMY_RESPAWN_DELAY = 3;

	//�G----------------------------------------------------------------
	std::array<unique_ptr<IEnemy>, 2>enemy;																					//�G(�T���v��)
	std::array<std::array<shared_ptr<IEnemy>, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX> m_enemies;	//1�X�e�[�W�ɐ�������G�̑���
	std::array<int, KazEnemyHelper::ENEMY_TYPE_MAX> enemiesHandle;															//0���珇�Ԃɏ���������ۂɕK�v
	std::array<int, KazEnemyHelper::ENEMY_TYPE_MAX> addEnemiesHandle;														//0���珇�Ԃɒǉ��ŏ���������ۂɕK�v
	std::array<std::array<ResponeData, KazEnemyHelper::ENEMY_NUM_MAX>, KazEnemyHelper::ENEMY_TYPE_MAX> m_responeData;		//�G�𐶐�����ۂɕK�v�Ȑݒ�
	std::array<ResponeData, 50>m_addResponeData;																			//�G��ǉ��Ő�������ۂɕK�v�Ȑݒ���X�^�b�N��������
	bool m_isEnemyNotMoveFlag;
	int m_notMoveTimer;
	const int CHANGE_GMAE_FLAME_SPEED_MAX_TIME = 2;	//�G�����Ȃ��Ȃ��Ă���Q�[�������Ԃ̐i�ރX�s�[�h�����܂�܂ł̊Ԋu
	std::vector<Sphere*>m_enemyHitBoxArray;

	//�G----------------------------------------------------------------

	//�C���Q�[��---------------------------------------

	//�f�o�b�N����---------------------------------------

	//�Q�[���J�n���̕\��
	//����
	DrawFuncData::DrawCallData m_guideRender;
	KazBufferHelper::BufferData m_infomationTex;
	int m_guideTimer;
	float m_guideAlphaTimer;

	//������@
	DrawFuncData::DrawCallData m_guideUI;
	KazBufferHelper::BufferData m_guideTex;


public:
	bool m_debugFlag;
private:
	DebugCamera m_debugCamera;
	//�f�o�b�N����---------------------------------------

	//�u���[���e�X�g
	DrawFuncData::DrawCallData m_bloomModelRender;

	int m_lightIndex = 0;

	RESOURCE_HANDLE m_bgmHandle;

	SoundData m_lockonSE;
};