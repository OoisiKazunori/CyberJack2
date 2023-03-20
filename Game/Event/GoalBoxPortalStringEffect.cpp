#include"GoalBoxPortalStringEffect.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

GoalBoxPortalStringEffect::GoalBoxPortalStringEffect() :startFlag(false), basePos({})
{
	for (int i = 0; i < stringRender.size(); ++i)
	{
		stringRender[i].data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::GoalPath + "StringEffect.png");
		stringRender[i].data.pipelineName = PIPELINE_NAME_SPRITE_MULTITEX;
	}
}

void GoalBoxPortalStringEffect::Init(const KazMath::Vec3<float> &POS)
{
	basePos = POS;
	for (int i = 0; i < stringRender.size(); ++i)
	{
		InitStringEffect(i);
	}

	startFlag = false;
}

void GoalBoxPortalStringEffect::Update()
{
	if (startFlag)
	{
		for (int i = 0; i < stringRender.size(); ++i)
		{
			float lVel = 1.0f;
			stringRender[i].data.transform.pos.z -= lVel;

			float resetLine = -80.0f;
			if (stringRender[i].data.transform.pos.z <= resetLine)
			{
				InitStringEffect(i);
			}
			stringRender[i].data.transform.scale = { 0.5f,0.5f,1.0f };
		}
	}

}

void GoalBoxPortalStringEffect::Draw()
{
	if (startFlag)
	{
		for (int i = 0; i < stringRender.size(); ++i)
		{
			stringRender[i].Draw();
		}
	}
}

void GoalBoxPortalStringEffect::Start()
{
	startFlag = true;
}

void GoalBoxPortalStringEffect::InitStringEffect(int INDEX)
{
	float minNum = 0.0f;
	float maxNum = 80.0f;
	float limitLine = 40.0f;

	KazMath::Vec3<float>lInitPos;
	lInitPos.x = basePos.x + KazMath::Rand<float>(maxNum, minNum);

	if (lInitPos.x <= limitLine)
	{
		lInitPos.y = basePos.y + KazMath::Rand<float>(maxNum, limitLine);
	}
	else
	{
		lInitPos.y = basePos.y + KazMath::Rand<float>(maxNum, minNum);
	}
	lInitPos.z = basePos.z + KazMath::Rand<float>(350.0f, 150.0f);


	int minusOrPlusNum = KazMath::Rand<int>(2, 0);
	if (minusOrPlusNum)
	{
		lInitPos.x *= -1.0f;
	}
	minusOrPlusNum = KazMath::Rand<int>(2, 0);
	if (minusOrPlusNum)
	{
		lInitPos.y *= -1.0f;
	}

	stringRender[INDEX].data.transform.pos = lInitPos;
	stringRender[INDEX].data.transform.scale = { 0.5f,0.5f,1.0f };
	stringRender[INDEX].data.transform.rotation.y = 90.0f;
	stringRender[INDEX].data.transform.rotation.z = KazMath::Rand<float>(360.0f, 0.0f);
}
