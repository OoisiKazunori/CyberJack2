#pragma once
#include"../KazLibrary/Render/KazRender.h"

/// <summary>
/// 1•¶Žš•ª‚Ì•`‰æ
/// </summary>
class Letter
{
public:
	Letter();
	void Init(const KazMath::Vec2<float> &POS, const char &CHARACTER, float FONT_SIZE, int STRING_INDEX);
	void Finalize();
	void Update(float Y_POS, int INDEX);
	void Draw();

private:
	KazMath::Vec2<float>basePos, baseSize;
	Sprite2DRender render, inputRender;

	enum FontType
	{
		CHARA_LARGE,
		CHARA_SMALL,
		CHARA_NUM,
		CHARA_SPECIAL,
		CHARA_MAX
	};
	bool CheckFontType(const char &CHARACTER, int MIN_NUM, int MAX_NUM);

	FontType fontType;
	int fontNum;
	std::array<RESOURCE_HANDLE, 4> graphHandle;
	RESOURCE_HANDLE inputLogHandle,testHandle;
	bool initFlag;
	bool spaceFlag;

	int timer;
	bool changeHandleFlag;

	int prevStringIndex;
};

class String
{
public:
	String();
	void Init(const KazMath::Vec2<float>POS, const std::string &STRING, float FONT_SIZE);
	void Finalize();
	void Update(int STRING_INDEX);
	void Draw();

	bool TimeOver()
	{
		return KazMath::ConvertSecondToFlame(10) <= timer;
	}
	const KazMath::Vec2<float> GetPos()
	{
		return stringPos;
	}
private:
	static const int LETTER_MAX_NUM = 20;
	std::array<Letter, LETTER_MAX_NUM>letters;

	int timer;
	int charaArrayNum;
	std::string logString;
	float fontSize;
	KazMath::Vec2<float>basePos;
	KazMath::Vec2<float>stringPos;
};