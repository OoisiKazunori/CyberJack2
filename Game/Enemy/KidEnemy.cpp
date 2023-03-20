#include "KidEnemy.h"

KidEnemy::KidEnemy()
{
}

void KidEnemy::Init(const EnemyGenerateData &GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_ObjModelRender->data.transform.pos = GENERATE_DATA.initPos;	//座標の初期化
	iEnemy_EnemyStatusData->hitBox.radius = 5.0f;	//当たり判定の大きさ変更
	iOperationData.Init(4,"Kid");							//残りロックオン数等の初期化

	iEnemy_ObjModelRender->data.colorData = { 255,0,0,255 };

	demoFlag = DEMO_FLAG;
}

void KidEnemy::Finalize()
{
}

void KidEnemy::Update()
{
	
}

void KidEnemy::Draw()
{
	//iEnemy_ModelRender->Draw();
}
