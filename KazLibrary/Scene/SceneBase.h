#pragma once
#include"SceneData.h"
class SceneBase
{
public:
	virtual ~SceneBase() = default;
	virtual void Init() = 0;
	virtual void PreInit() {};
	virtual void Finalize() = 0;
	virtual void Input() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	

	virtual int SceneChange() = 0;

	bool endGameFlag;
	bool skipTurtorialFlag;
	bool firstGenerateFlag;
protected:

};