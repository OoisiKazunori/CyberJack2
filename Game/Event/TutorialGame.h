#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../Game/UI/Letter.h"
#include"../Game/Event/WindowFlame.h"

class TutorialGame
{
public:
	TutorialGame();
	void Init(const std::string &TEXT);
	void Finalize();
	void Update();
	void Draw();

	void Succeed();
	bool NextTutorial();

private:
	KazMath::Vec3<float>basePos;
	WindowFlame window;
	std::vector<std::unique_ptr<String>> tutorialText;

	bool endFlag;
	int endTimer;
	bool readyToWriteFlag;

	std::vector<std::string> textArray;
};