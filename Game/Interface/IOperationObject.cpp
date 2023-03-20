#include "IOperationObject.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

IOperationObject::IOperationObject()
{
	lockOnWindowRender.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::LinePath + "LockOn.png");
	lockOnWindowRender.data.transform.scale = { 0.2f,0.2f };
	//lockOnWindowRender.data.billBoardFlag = true;
	lockOnWindowRender.data.pipelineName = PIPELINE_NAME_SPRITE_Z_ALWAYS;
}

void IOperationObject::Hit()
{
	--iOperationData.rockOnNum;
}

bool IOperationObject::LockedOrNot()
{
	return iOperationData.rockOnNum <= 0;
}

bool IOperationObject::IsAlive()
{
	return iOperationData.enableToHitFlag && !LockedOrNot();
}

int IOperationObject::ReduceHp(int COUNT)
{
	if (!startToReduceHpFlag)
	{
		reduceHpNum = COUNT;
		startToReduceHpFlag = true;
	}
	//一定時間の間隔で指定した回数分体力を減らす
	if (startToReduceHpFlag)
	{
		++timer;
		if (5 <= timer)
		{
			--iOperationData.hp;
			--reduceHpNum;
			timer = 0;
			return 1;
		}
	}
	//回数分終えたら終了する
	if (reduceHpNum <= 0)
	{
		startToReduceHpFlag = false;
		return 2;
	}
	return 0;
}

void IOperationObject::LockOnWindow(const KazMath::Vec3<float> &POS)
{
	if (iOperationData.lockOnFlag && iOperationData.enableToHitFlag)
	{
		KazMath::Vec3<float>lScreenPos =
			KazMath::ConvertWorldPosToScreenPos(POS, CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
		//敵が画面外に出た際に画面横からウィンドウが出ないように変換後のz値が1.0以上は描画しない
		if (lScreenPos.z <= 1.0f)
		{
			lockOnWindowRender.data.transform.pos = KazMath::Vec2<float>(lScreenPos.x, lScreenPos.y);
			lockOnWindowRender.Draw();
		}
	}
}
