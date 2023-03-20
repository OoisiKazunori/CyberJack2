#include"PopEnemy.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

PopEnemy::PopEnemy()
{
}

void PopEnemy::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	InitModel(KazMath::Transform3D(GENERATE_DATA.initPos, { 1.0f,1.0f,1.0f }, { 0.0f,180.0f,0.0f }), KazFilePathName::EnemyPath + "PopEnemy/" + "PopEnemy_Model.fbx", 15.0f, ENEMY_MODEL_FBX, true, true);
	iOperationData.Init(1, "snkr_BR");							//残りロックオン数等の初期化

	iEnemy_FbxModelRender->data.colorData.color.a = 255;

	initDeadSoundFlag = false;
	demoFlag = DEMO_FLAG;

	basePos = GENERATE_DATA.initPos;
	bottomPos = basePos + KazMath::Vec3<float>(0.0f, -200.0f, 0.0f);

	reverseFlag = false;
	vel.y = 0.0f;


	iEnemy_EnemyStatusData->objFlag = false;

	if (demoFlag)
	{
		vel.z = 0.0f;
		vel.x = 0.0f;
	}
	else
	{
		if (GENERATE_DATA.popEnemyData.leftRightFlag)
		{
			vel.x = -0.5f;
		}
		else
		{
			vel.x = 0.5f;
		}
		vel.z = -0.1f;
	}

	firstTimeToJumpFlag = false;
}

void PopEnemy::Finalize()
{
}

void PopEnemy::Update()
{
	if (reverseFlag && vel.y <= 0.0f)
	{
		reverseFlag = false;
	}
	if (iEnemy_FbxModelRender->data.transform.pos.y <= bottomPos.y)
	{
		reverseFlag = true;
		vel.x += 0.1f;
		vel.x *= -1.0f;
		vel.z += -0.1f;

		firstTimeToJumpFlag = true;
	}

	if (firstTimeToJumpFlag)
	{
		vel.y += -0.15f;
	}
	else
	{
		vel.y += -0.1f;
	}


	if (reverseFlag != oldReverseFlag)
	{
		vel.y *= -1.0f;
	}
	oldReverseFlag = reverseFlag;


	iEnemy_FbxModelRender->data.transform.pos += vel;

	ProcessingOfDeath(DEATH_ROLL);
}

void PopEnemy::Draw()
{
	if (0 < iEnemy_FbxModelRender->data.colorData.color.a)
	{
		iEnemy_FbxModelRender->Draw();
		LockOnWindow(iEnemy_FbxModelRender->data.transform.pos);
	}
}
