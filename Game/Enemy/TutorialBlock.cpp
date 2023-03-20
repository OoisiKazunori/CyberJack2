#include"TutorialBlock.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

TutorialBlock::TutorialBlock()
{
	redBlockResourceHandle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::TutorialPath + "TutorialRedBlock.obj");
	greenBlockResourceHandle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::TutorialPath + "TutorialGreenBlock.obj");
}

void TutorialBlock::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	ReleaseLight();

	iEnemy_ObjModelRender->data.transform.pos = GENERATE_DATA.initPos;	//座標の初期化
	iEnemy_ObjModelRender->data.transform.scale = { 0.0f,0.0f,0.0f };
	iEnemy_ObjModelRender->data.handle = redBlockResourceHandle;
	iEnemy_EnemyStatusData->hitBox.center = &iEnemy_ObjModelRender->data.transform.pos;
	iEnemy_EnemyStatusData->hitBox.radius = 5.0f;	//当たり判定の大きさ変更
	iOperationData.Init(1, "KeyBlock");							//残りロックオン数等の初期化

	iEnemy_ObjModelRender->data.pipelineName = PIPELINE_NAME_OBJ_MULTITEX;
	iEnemy_ObjModelRender->data.removeMaterialFlag = false;
	iEnemy_ObjModelRender->data.colorData.color.x = 255;
	iEnemy_ObjModelRender->data.colorData.color.y = 255;
	iEnemy_ObjModelRender->data.colorData.color.z = 255;
	iEnemy_ObjModelRender->data.colorData.color.a = 255;
	iEnemy_ObjModelRender->data.transform.rotation.x = 0.0f;
	iEnemy_ObjModelRender->data.transform.rotation.y = 180.0f;
	iEnemy_ObjModelRender->data.transform.rotation.z = 0.0f;
	initDeadSoundFlag = false;
	demoFlag = DEMO_FLAG;
	iEnemy_EnemyStatusData->radius = 8.0f;
	iEnemy_EnemyStatusData->startFlag = true;

	larpScale = { 1.5f,1.5f,1.5f };
	marker.Init(*iEnemy_EnemyStatusData->hitBox.center);

	baseScale = 0.5f;
	deadFlag = false;
}

void TutorialBlock::Finalize()
{
	deadFlag = true;
	timer = 0;
}

void TutorialBlock::Update()
{

	KazMath::Larp(larpScale, &iEnemy_ObjModelRender->data.transform.scale, 0.1f);
	if (!deadFlag)
	{
		++timer;
		if (60 <= timer && iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
		{
			iEnemy_ObjModelRender->data.transform.scale = { baseScale,baseScale,baseScale };
			timer = 0;
			larpScale = { 1.5f,1.5f,1.5f };
			baseScale = 0.5f;
			iEnemy_ObjModelRender->data.handle = redBlockResourceHandle;
		}
		else if (30 <= timer && !iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
		{
			iEnemy_ObjModelRender->data.transform.scale = { baseScale,baseScale,baseScale };
			timer = 0;
			larpScale = { 2.0f,2.0f,2.0f };
			baseScale = 1.0f;
			iEnemy_ObjModelRender->data.handle = greenBlockResourceHandle;
		}

		marker.Update();
	}
	else
	{
		++timer;
		if (timer <= 30)
		{
			larpScale = { 3.0f,3.0f,3.0f };
		}
		else
		{
			larpScale = { 0.0f,0.0f,0.0f };
		}
		
	}
}

void TutorialBlock::Draw()
{
	if (1.0f <= iEnemy_ObjModelRender->data.colorData.color.a)
	{
		iEnemy_ObjModelRender->Draw();
		LockOnWindow(iEnemy_ObjModelRender->data.transform.pos);
	}

	if (deadFlag)
	{
		return;
	}

	if (iEnemy_EnemyStatusData->oprationObjData->enableToHitFlag)
	{
		marker.Draw();
	}
}