#pragma once
#include"../Game/UI/Letter.h"
#include"../KazLibrary/Math/KazMath.h"

class MenuRender
{
public:
	MenuRender();
	void Init(const KazMath::Vec2<float> &POS, const std::string &MENU_STRING);
	void Finalize();
	void Update(int SELECT_NUM);
	void Draw();
	static void ResetMenuIndex() { menuIndex = 0; };

private:
	String string;
	Sprite2DRender menuFlashR;
	static int menuIndex;
	int menuNowIndex;

	bool reverseFlag;
	KazMath::Vec2<float>flashPos;
	float stringSize;

	bool finishFlag;
};