#include "MoiveEffect.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

MoiveEffect::MoiveEffect() :startFlag(false), lerpTopPos({ 0.0f,0.0f }), lerpButtomPos({ 0.0f,WIN_Y })
{
	topTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "Black.png");
	buttomTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "Black.png");

	topTex.data.transform.scale = { 1280.0f,2.0f };
	buttomTex.data.transform.scale = { 1280.0f,2.0f };
}

void MoiveEffect::Init()
{
	startFlag = false;

	float texSpace = 50.0f;
	lerpTopPos = { 0.0f,-texSpace };
	lerpButtomPos = { 0.0f,WIN_Y + texSpace };

	topTex.data.transform.pos = lerpTopPos;
	buttomTex.data.transform.pos = lerpButtomPos;
}

void MoiveEffect::Update()
{
	float space = 5.0f;
	//演出開始
	if (startFlag)
	{
		lerpTopPos = { 0.0f,0.0f + space };
		lerpButtomPos = { 0.0f,WIN_Y - space };
	}
	//演出終了
	else
	{
		float texSpace = 50.0f;
		lerpTopPos = { 0.0f,-texSpace };
		lerpButtomPos = { 0.0f,WIN_Y + texSpace };
	}

	KazMath::Larp(lerpTopPos.y, &topTex.data.transform.pos.y, 0.1f);
	KazMath::Larp(lerpButtomPos.y, &buttomTex.data.transform.pos.y, 0.1f);


	topTex.data.transform.scale = { 1280.0f,10.0f };
	buttomTex.data.transform.scale = { 1280.0f,10.0f };
}

void MoiveEffect::Draw()
{
	topTex.Draw();
	buttomTex.Draw();
}
