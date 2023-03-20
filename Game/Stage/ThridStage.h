#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Interface/IStage.h"

class ThridStage :public IStage
{
public:
	ThridStage();
	void Init();
	void Update();
	void Draw();

private:
	Sprite3DRender infomationTex;
	Sprite3DRender pressAButtonTex;
	int timer;
	bool flashFlag;
};

