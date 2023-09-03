#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"TitleScene.h"
#include"GameScene.h"
#include"DemoScene.h"
#include"../Scene/SceneChange.h"
#include"RenderTarget/GaussianBuler.h"
#include"RenderTarget/RenderTargetStatus.h"
#include<memory>
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include "../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Raytracing/RayPipeline.h"


class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void Update();
	void Draw();

	bool endGameFlag;
private:
	std::vector<std::unique_ptr<SceneBase>> scene;
	std::unique_ptr<ChangeScene::SceneChange> change;
	int nowScene, nextScene;
	bool initGameFlag;

	bool sceneChangeFlag;

	bool itisInArrayFlag;
	bool gameFirstInitFlag;


	SoundData m_BGN;


	//�f�o�b�O�֘A�̕ϐ�
	bool m_isDebugCamera;
	bool m_isDebugRaytracing;
	bool m_isDebugTimeZone;
	bool m_isDebugVolumeFog;
	bool m_isDebugSea;


	//���X�^���C�U�`��
	DrawingByRasterize m_rasterize;

	//���C�g���Ƀ��f����g�ݍ��ޗp�̔z��N���X
	Raytracing::BlasVector m_blasVector;
	//���C�g���ŕ`�悷�邽�߂̏��
	Raytracing::Tlas m_tlas;
	//���C�g���p�p�C�v���C��
	std::vector<Raytracing::RayPipelineShaderData> m_pipelineShaders;
	std::unique_ptr<Raytracing::RayPipeline> m_rayPipeline;
};