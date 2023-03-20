#include "IEnemy.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../Game/Effect/InstanceMeshParticle.h"

IEnemy::IEnemy() :hpDirtyFlag(&iOperationData.rockOnNum)
{
	//•`‰æ‚Ì‰Šú‰»----------------------------------------------------------------
	iEnemy_ObjModelRender = std::make_unique<ObjModelRender>();
	iEnemy_ObjModelRender->data.pipelineName = PIPELINE_NAME_OBJ_MULTITEX_LIGHT;

	iEnemy_FbxModelRender = std::make_unique<FbxModelRender>();
	iEnemy_FbxModelRender->data.pipelineName = PIPELINE_NAME_FBX_RENDERTARGET_TWO_LIGHT;

	fbxLightHandle = iEnemy_FbxModelRender->CreateConstBuffer(sizeof(DirectX::XMFLOAT3), typeid(DirectX::XMFLOAT3).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA3);
	DirectX::XMFLOAT3 dir = { -1.0f,0.0f,0.0f };
	iEnemy_FbxModelRender->TransData(&dir, fbxLightHandle, typeid(DirectX::XMFLOAT3).name());

	objLightHandle = iEnemy_ObjModelRender->CreateConstBuffer(sizeof(DirectX::XMFLOAT3), typeid(DirectX::XMFLOAT3).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
	iEnemy_FbxModelRender->TransData(&dir, objLightHandle, typeid(DirectX::XMFLOAT3).name());
	//•`‰æ‚Ì‰Šú‰»----------------------------------------------------------------

	//“Gî•ñ‚Ì‰Šú‰»----------------------------------------------------------------
	iEnemy_EnemyStatusData = std::make_unique<EnemyData>(
		&iEnemy_ObjModelRender->data.transform.pos,
		-1.0f,
		&iOperationData
		);
	//“Gî•ñ‚Ì‰Šú‰»----------------------------------------------------------------

	lockOnWindowRender.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::LinePath + "LockOn.png");
	lockOnWindowRender.data.transform.scale = { 0.3f,0.3f };
	lockOnWindowRender.data.pipelineName = PIPELINE_NAME_SPRITE_Z_ALWAYS;

	deadSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "EnemyDead.wav", false);
	shotSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "Shot.wav", false);

	debugShotFlag = false;


	initDeadRotaVel = { KazMath::Rand(5.0f,-5.0f),KazMath::Rand(5.0f,-5.0f) ,KazMath::Rand(5.0f,-5.0f) };
	initDeadYVel = { KazMath::Rand(1.0f,-0.5f),-KazMath::Rand(1.5f,0.5f),KazMath::Rand(1.0f,-0.5f) };

	iEnemy_FbxModelRender->data.transform.pos = { 0.0f,0.0f,-10000.0f };
	iEnemy_ObjModelRender->data.transform.pos = { 0.0f,0.0f,-10000.0f };

	yVel = KazMath::Rand<float>(1.0f, 0.1f);
}

void IEnemy::Dead()
{
	iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
}

void IEnemy::DeadEffect(KazMath::Vec3<float> *POS, KazMath::Vec3<float> *ROTATION, int *ALPHA)
{
	if (*ALPHA <= 0)
	{
		*ALPHA = 0;
	}
	else
	{
		KazMath::Vec3<float> rota{ 5.0f,5.0f,5.0f };
		*ROTATION += initDeadRotaVel;
		POS->y -= yVel;
		*ALPHA -= 5;
	}
}

bool IEnemy::ProcessingOfDeath(EnemyDeathType TYPE)
{
	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		iEnemy_ObjModelRender->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
		iEnemy_ObjModelRender->data.removeMaterialFlag = true;
		iEnemy_ObjModelRender->data.colorData.color.x = 255;
		iEnemy_ObjModelRender->data.colorData.color.y = 255;
		iEnemy_ObjModelRender->data.colorData.color.z = 255;

		if (!initDeadSoundFlag)
		{
			DeadSound();
			initDeadSoundFlag = true;

			iEnemy_ObjModelRender->Release(objLightHandle);
		}

		switch (TYPE)
		{
		case DEATH_ROLL:
			DeadEffect(&iEnemy_ObjModelRender->data.transform.pos, &iEnemy_ObjModelRender->data.transform.rotation, &iEnemy_ObjModelRender->data.colorData.color.a);
			break;
		case DEATH_SINK:
			iEnemy_ObjModelRender->data.transform.pos.y -= 0.5f;
			iEnemy_ObjModelRender->data.transform.rotation.x += 0.5f;
			iEnemy_ObjModelRender->data.colorData.color.a -= 5;
			break;
		default:
			break;
		}
	}

	if (iEnemy_ObjModelRender->data.colorData.color.a <= 0)
	{
		iEnemy_ObjModelRender->data.colorData.color.a = 0;
		iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
		iEnemy_EnemyStatusData->outOfStageFlag = true;
	}

	return !iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag;
}

bool IEnemy::ProcessingOfDeathFbx(EnemyDeathType TYPE)
{
	if (!iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		iEnemy_FbxModelRender->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
		iEnemy_FbxModelRender->data.removeMaterialFlag = true;
		iEnemy_FbxModelRender->data.colorData.color.x = 255;
		iEnemy_FbxModelRender->data.colorData.color.y = 255;
		iEnemy_FbxModelRender->data.colorData.color.z = 255;

		if (!initDeadSoundFlag)
		{
			DeadSound();
			initDeadSoundFlag = true;

			iEnemy_FbxModelRender->Release(fbxLightHandle);
			iEnemy_FbxModelRender->ReleaseSkining();
		}


		switch (TYPE)
		{
		case DEATH_ROLL:
			DeadEffect(&iEnemy_FbxModelRender->data.transform.pos, &iEnemy_FbxModelRender->data.transform.rotation, &iEnemy_FbxModelRender->data.colorData.color.a);
			break;
		case DEATH_SINK:
			iEnemy_FbxModelRender->data.transform.pos.y -= 0.5f;
			iEnemy_FbxModelRender->data.transform.rotation.x += 0.5f;
			iEnemy_FbxModelRender->data.colorData.color.a -= 5;
			break;
		default:
			break;
		}
	}

	if (iEnemy_FbxModelRender->data.colorData.color.a <= 0)
	{
		iEnemy_FbxModelRender->data.colorData.color.a = 0;
		iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag = false;
		iEnemy_EnemyStatusData->outOfStageFlag = true;
	}

	return !iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag;
}

const unique_ptr<EnemyData> &IEnemy::GetData()
{
	return iEnemy_EnemyStatusData;
}

void IEnemy::DeadSound()
{
	SoundManager::Instance()->PlaySoundMem(deadSoundHandle, 1);
}

void IEnemy::ShotSound()
{
	SoundManager::Instance()->PlaySoundMem(shotSoundHandle, 1);
}

void IEnemy::InitModel(const KazMath::Transform3D &TRANSFORM, const std::string &MODEL_FILEPASS, float HITBOX_RADIUS, EnemyModelType MODEL_TYPE, bool REV_UV_FLAG, bool GENERATE_PARTICLE_FLAG)
{
	modelType = MODEL_TYPE;
	switch (MODEL_TYPE)
	{
	case ENEMY_MODEL_NONE:
		break;
	case ENEMY_MODEL_OBJ:
		iEnemy_ObjModelRender->data.handle = ObjResourceMgr::Instance()->LoadModel(MODEL_FILEPASS);	//ƒ‚ƒfƒ‹“Ç‚İ‚İ
		iEnemy_ObjModelRender->data.transform = TRANSFORM;
		iEnemy_EnemyStatusData->hitBox.radius = HITBOX_RADIUS;	//“–‚½‚è”»’è‚Ì‘å‚«‚³•ÏX
		iEnemy_EnemyStatusData->hitBox.center = &iEnemy_ObjModelRender->data.transform.pos;
		iEnemy_ObjModelRender->data.pipelineName = PIPELINE_NAME_OBJ_MULTITEX_LIGHT;

		iEnemy_ObjModelRender->data.removeMaterialFlag = false;
		iEnemy_ObjModelRender->data.colorData.color.x = 255;
		iEnemy_ObjModelRender->data.colorData.color.y = 255;
		iEnemy_ObjModelRender->data.colorData.color.z = 255;
		iEnemy_ObjModelRender->data.colorData.color.a = 1;

		break;
	case ENEMY_MODEL_FBX:
		iEnemy_FbxModelRender->data.handle = FbxModelResourceMgr::Instance()->LoadModel(MODEL_FILEPASS, REV_UV_FLAG);	//ƒ‚ƒfƒ‹“Ç‚İ‚İ
		iEnemy_FbxModelRender->data.transform = TRANSFORM;
		iEnemy_EnemyStatusData->hitBox.radius = HITBOX_RADIUS;	//“–‚½‚è”»’è‚Ì‘å‚«‚³•ÏX
		iEnemy_EnemyStatusData->hitBox.center = &iEnemy_FbxModelRender->data.transform.pos;
		iEnemy_FbxModelRender->data.pipelineName = PIPELINE_NAME_FBX_RENDERTARGET_TWO_LIGHT;

		iEnemy_FbxModelRender->data.removeMaterialFlag = false;
		iEnemy_FbxModelRender->data.colorData.color.x = 255;
		iEnemy_FbxModelRender->data.colorData.color.y = 255;
		iEnemy_FbxModelRender->data.colorData.color.z = 255;
		iEnemy_FbxModelRender->data.colorData.color.a = 1;

		iEnemy_FbxModelRender->data.isPlayFlag = false;

		iEnemy_EnemyStatusData->alpha = &iEnemy_FbxModelRender->data.colorData.color.a;
		iEnemy_EnemyStatusData->motherMat = &iEnemy_FbxModelRender->motherMat;


		if (!GENERATE_PARTICLE_FLAG)
		{
			return;
		}

		MeshParticleLoadData lData;
		lData.bias = 70;
		lData.faceCountNum = 100;
		lData.perTriangleNum = 50;

		iEnemy_EnemyStatusData->meshParticleData.emplace_back(
			std::make_shared<MeshData>(0, &iEnemy_FbxModelRender->motherMat)
		);
		iEnemy_EnemyStatusData->meshParticleData[0]->meshParticleData =
			MeshParticleLoader::Instance()->Load(
				MODEL_FILEPASS,
				REV_UV_FLAG,
				&iEnemy_FbxModelRender->motherMat,
				lData
			);

		iEnemy_EnemyStatusData->deadParticleData.motherMat = iEnemy_EnemyStatusData->meshParticleData[0]->meshParticleData.motherMat;
		iEnemy_EnemyStatusData->deadParticleData.startFlag = &iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag;
		iEnemy_EnemyStatusData->deadParticleData.meshParticleData = iEnemy_EnemyStatusData->meshParticleData[0]->meshParticleData;
		break;
	case ENEMY_MODEL_MESHPARTICLE:
		//RESOURCE_HANDLE lHandle = FbxModelResourceMgr::Instance()->LoadModel(MODEL_FILEPASS, REV_UV_FLAG);	//ƒ‚ƒfƒ‹“Ç‚İ‚İ)
		//iEnemy_MeshModelRender = std::make_unique<MeshParticleEmitter>(FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertData);
		//iEnemy_MeshModelRender->Init();
		break;
	default:
		break;
	}
}
