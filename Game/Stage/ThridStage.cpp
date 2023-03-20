#include "ThridStage.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

ThridStage::ThridStage()
{
	infomationTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "Devoloping.png");
	infomationTex.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA_MULTITEX;

	pressAButtonTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "PressAButton.png");
	pressAButtonTex.data.pipelineName = PIPELINE_NAME_SPRITE_CUTALPHA_MULTITEX;
}

void ThridStage::Init()
{
	timer = 0;
	flashFlag = false;
}

void ThridStage::Update()
{
	pressAButtonTex.data.transform.pos = { 0.0f,-100.0f,700.0f };
	pressAButtonTex.data.transform.scale = { 0.8f,0.8f,1.0f };
	infomationTex.data.transform.pos = { 0.0f,0.0f,700.0f };

	++timer;
	if (timer % 60 == 0)
	{
		flashFlag = !flashFlag;
	}
}

void ThridStage::Draw()
{
	infomationTex.Draw();
	if (flashFlag)
	{
		pressAButtonTex.Draw();
	}
}
