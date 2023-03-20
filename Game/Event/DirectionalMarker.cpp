#include "DirectionalMarker.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

DirectionalMarker::DirectionalMarker()
{
	dir3DRender.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "DirectionalMarker.png");
	dir2DRender.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "DirectionalMarker.png");

	dir2DRender.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;
	dir3DRender.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA;
	initFlag = false;
}

void DirectionalMarker::Init(const KazMath::Vec3<float> &OBJ_POS)
{
	objPos = OBJ_POS;
	dir3DRender.data.transform.pos = objPos + KazMath::Vec3<float>(-5.0f, 4.0f, 0.0f);
	dir3DRender.data.transform.scale = { 0.4f,0.4f,0.4f };
	dir3DRender.data.transform.rotation = { 0.0f,0.0f,-135.0f };
	dir3DRender.data.billBoardFlag = true;

	dir2DRender.data.transform.scale = { 3.0f,3.0f };
	initFlag = true;
	timer = 0.0f;
}

void DirectionalMarker::Update()
{
	if (!initFlag)
	{
		return;
	}

	//対象のオブジェクトが範囲内にいるかどうか
	KazMath::Vec3<float>lObjScreenPos =
		KazMath::ConvertWorldPosToScreenPos(objPos, CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection());
	if (0.0f <= lObjScreenPos.x && 0.0f <= lObjScreenPos.y && lObjScreenPos.x <= WIN_X && lObjScreenPos.y <= WIN_Y)
	{
		isInScreenFlag = true;
	}
	else
	{
		isInScreenFlag = false;
	}


	//2D上の矢印の挙動
	{
		dir2DRender.data.transform.pos = { lObjScreenPos.x, lObjScreenPos.y };
		KazMath::Vec2<float>lPos = dir2DRender.data.transform.pos;
		if (lPos.x < 50.0f)
		{
			dir2DRender.data.transform.pos.x = 50.0f;
		}
		if (lPos.y < 50.0f)
		{
			dir2DRender.data.transform.pos.y = 50.0f;
		}
		if ((WIN_X - 50.0f) < lPos.x)
		{
			dir2DRender.data.transform.pos.x = WIN_X - 50.0f;
		}
		if ((WIN_Y - 50.0f) < lPos.y)
		{
			dir2DRender.data.transform.pos.y = WIN_Y - 50.0f;
		}

		KazMath::Vec2<float>dirVec = dir2DRender.data.transform.pos - KazMath::Vec2<float>(lObjScreenPos.x, lObjScreenPos.y);
		dirVec.Normalize();
		dir2DRender.data.transform.rotation = static_cast<float>(KazMath::RadianToAngle(atan2(dirVec.x, dirVec.y)));
		dir2DRender.data.transform.pos += ((dirVec * 10.0f) * sinf(KazMath::PI_2F / 120 * timer));
		timer += 5.0f;



		dir3DRender.data.transform.scale;

		if (objPos.x <= 0.0f)
		{
			dir2DRender.data.transform.rotation *= -1.0f;
		}
	}
}

void DirectionalMarker::Draw()
{
	if (!initFlag)
	{
		return;
	}

	if (isInScreenFlag)
	{
		dir3DRender.Draw();
	}
	else
	{
		dir2DRender.Draw();
	}
}
