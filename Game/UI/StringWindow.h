#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../Game/UI/Letter.h"
#include"../Game/Event/WindowFlame.h"

class StringWindow
{
public:
	StringWindow();
	void Init(const std::string &TEXT);
	void Finalize();
	void Update();
	void Draw();

	bool IsFinish();

private:
	KazMath::Vec3<float>basePos;
	WindowFlame window;
	String tutorialText;

	bool endFlag;
	int endTimer;
	bool readyToWriteFlag;
	bool initFlag;

	std::string text;
};

