#include "Letter.h"
#include<assert.h>
#include"../KazLibrary/Helper/ResourceFilePass.h"

Letter::Letter()
{
	graphHandle[CHARA_LARGE] = TextureResourceMgr::Instance()->LoadDivGraph(KazFilePathName::UIPath + "UppercaseAlphabet.png", 26, 1, 16, 16);
	graphHandle[CHARA_SMALL] =  TextureResourceMgr::Instance()->LoadDivGraph(KazFilePathName::UIPath + "LowercaseAlphabet.png", 26, 1, 16, 16);
	graphHandle[CHARA_NUM] = TextureResourceMgr::Instance()->LoadDivGraph(KazFilePathName::UIPath + "Number.png", 10, 1, 16, 16);
	graphHandle[CHARA_SPECIAL] = TextureResourceMgr::Instance()->LoadDivGraph(KazFilePathName::UIPath + "SpecialKey.png", 4, 1, 16, 16);
	inputLogHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::UIPath + "inputLog.png");
	inputRender.data.handleData = inputLogHandle;
	inputRender.data.transform.scale = { 1.0f,1.5f };
	inputRender.data.pipelineName = PIPELINE_NAME_SPRITE_Z_ALWAYS_CUTALPHA;
	initFlag = false;
}

void Letter::Init(const KazMath::Vec2<float> &POS, const char &CHARACTER, float FONT_SIZE, int STRING_INDEX)
{
	basePos = POS;
	baseSize = { FONT_SIZE ,FONT_SIZE };

	const float L_ADD_SCALE = 0.3f;
	render.data.transform.pos = basePos + KazMath::Vec2<float>(10.0f, 10.0f);
	render.data.transform.scale = baseSize + L_ADD_SCALE;
	render.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::UIPath + "FontTest.png");
	render.data.pipelineName = PIPELINE_NAME_SPRITE_COLOR;
	render.data.colorData.color = { 153,229,80,255 };

	inputRender.data.transform.pos = basePos;

	spaceFlag = false;

	//文字を
	if (CheckFontType(CHARACTER, 65, 90))
	{
		fontType = CHARA_LARGE;
		fontNum = CHARACTER - 65;
	}
	else if (CheckFontType(CHARACTER, 97, 123))
	{
		fontType = CHARA_SMALL;
		fontNum = CHARACTER - 97;
	}
	else if (CheckFontType(CHARACTER, 48, 58))
	{
		fontType = CHARA_NUM;
		fontNum = CHARACTER - 48;
	}
	else if (CHARACTER == 32)
	{
		spaceFlag = true;
	}
	else
	{
		//特殊なキー(必要最低限のもの)
		switch (CHARACTER)
		{
		case 45:
			fontType = CHARA_SPECIAL;
			fontNum = 0;
			break;
		case 91:
			fontType = CHARA_SPECIAL;
			fontNum = 1;
			break;
		case 93:
			fontType = CHARA_SPECIAL;
			fontNum = 2;
			break;
		case 95:
			fontType = CHARA_SPECIAL;
			fontNum = 3;
			break;
		default:
			//使えない文字が入力されました
			assert(0);
			break;
		}
	}


	changeHandleFlag = false;
	initFlag = true;

	timer = 0;
	prevStringIndex = STRING_INDEX;

	if (spaceFlag)
	{
		return;
	}
	render.data.handleData = graphHandle[fontType];
	render.data.animationHandle = fontNum;
}

void Letter::Finalize()
{
	initFlag = false;
}

void Letter::Update(float Y_POS, int INDEX)
{
	if (!initFlag)return;

	//文のインデックスが変わったら文字の座標を変える
	if (INDEX != prevStringIndex)
	{
		/*
		基本座標と補間座標の差分を出す
		↓
		基本座標と補間座標を変更後の同じ場所に
		↓
		差分を補間座標に足す
		*/
		KazMath::Vec2<float>sub = basePos - render.data.transform.pos;
		basePos.y = Y_POS;
		render.data.transform.pos = basePos + sub;

		prevStringIndex = INDEX;
	}

	//補間をかける
	KazMath::Larp(basePos, &render.data.transform.pos, 0.3f);
	KazMath::Larp(baseSize, &render.data.transform.scale, 0.3f);

	++timer;
	if (2 <= timer)
	{
		changeHandleFlag = true;
	}
}

void Letter::Draw()
{
	if (!initFlag)return;

	if (!changeHandleFlag)
	{
		inputRender.Draw();
	}
	else if (!spaceFlag)
	{
		render.Draw();
	}
}

bool Letter::CheckFontType(const char &CHARACTER, int MIN_NUM, int MAX_NUM)
{
	for (int i = MIN_NUM; i < MAX_NUM; ++i)
	{
		if (i == CHARACTER)
		{
			return true;
		}
	}
	return false;
}


String::String()
{
}

void String::Init(const KazMath::Vec2<float>POS, const std::string &STRING, float FONT_SIZE)
{
	logString = STRING;
	fontSize = FONT_SIZE;
	timer = 4;
	charaArrayNum = 0;

	basePos = POS;

}

void String::Finalize()
{
	for (int i = 0; i < letters.size(); ++i)
	{
		letters[i].Finalize();
	}
}

void String::Update(int STRING_INDEX)
{
	//時間経過で文字を一文字ずつ出す処理ーーー
	bool lIsStringInArraySizeFlag = charaArrayNum <= logString.size()-1 && logString.size() != 0;
	if (1 <= timer && lIsStringInArraySizeFlag && charaArrayNum < letters.size())
	{
		timer = 0;
		KazMath::Vec2<float>lPos = { basePos.x + static_cast<float>(charaArrayNum) * (16.0f * fontSize), basePos.y + 20.0f * static_cast<float>(STRING_INDEX) };
		letters[charaArrayNum].Init(lPos, logString[charaArrayNum], fontSize, STRING_INDEX);
		++charaArrayNum;
	}
	++timer;
	//時間経過で文字を一文字ずつ出す処理ーーー

	float lYPos = basePos.y + 20.0f * static_cast<float>(STRING_INDEX);
	for (int i = 0; i < letters.size(); ++i)
	{
		letters[i].Update(lYPos, STRING_INDEX);
	}
	stringPos = { basePos.x,lYPos };
}

void String::Draw()
{
	for (int i = 0; i < letters.size(); ++i)
	{
		letters[i].Draw();
	}
}
