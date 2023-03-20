#include "DrawMenu.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

int MenuRender::menuIndex = 0;

MenuRender::MenuRender()
{
	menuFlashR.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::UIPath + "Line.png");
	menuFlashR.data.colorData.color.a = 0;
	menuNowIndex = menuIndex;
	++menuIndex;
}

void MenuRender::Init(const KazMath::Vec2<float> &POS, const std::string &MENU_STRING)
{
	float fontScale = 1.5f;
	string.Init(POS, MENU_STRING, fontScale);
	flashPos = { (16.0f * fontScale) * (MENU_STRING.size() / 2), 0.0f };
	stringSize = static_cast<float>(MENU_STRING.size());
	finishFlag = false;
}

void MenuRender::Finalize()
{
	finishFlag = true;
}

void MenuRender::Update(int SELECT_NUM)
{
	if (SELECT_NUM == menuNowIndex)
	{
		if (200 <= menuFlashR.data.colorData.color.a)
		{
			menuFlashR.data.colorData.color.a = 200;
			reverseFlag = true;
		}
		if (menuFlashR.data.colorData.color.a <= 150)
		{
			menuFlashR.data.colorData.color.a = 150;
			reverseFlag = false;
		}

		int lFlashRate = 60 / 10;
		if (reverseFlag)
		{
			menuFlashR.data.colorData.color.a += -lFlashRate;
		}
		else
		{
			menuFlashR.data.colorData.color.a += lFlashRate;
		}
	}
	else
	{
		menuFlashR.data.colorData.color.a = 0;
	}

	string.Update(menuNowIndex);

	
	menuFlashR.data.transform.pos = string.GetPos() + flashPos;
	menuFlashR.data.transform.scale = { stringSize * 1.5f - 1.1f,1.2f };

	if (finishFlag)
	{
		string.Finalize();
		menuFlashR.data.colorData.color.a = 0;
	}
}

void MenuRender::Draw()
{
	string.Draw();
	menuFlashR.Draw();
}
