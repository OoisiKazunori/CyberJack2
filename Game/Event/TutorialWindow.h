#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Event/WindowFlame.h"

class TutorialWindow
{
public:
	TutorialWindow();
	void Init();
	void Update();
	void Draw();

	void End();
private:
	WindowFlame windowFlame;

	Sprite3DRender lStickUi,abuttonUi,attackUi;
	LineRender line;
	bool endFlag;
};
