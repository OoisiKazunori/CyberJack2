#pragma once
#include"../KazLibrary/Helper/ISinglton.h"

class GameCommonData:public ISingleton<GameCommonData>
{
public:
	GameCommonData()
	{
		IsSecondFlag = false;
	}
	//既にゲームを起動したかどうか
	bool IsSecondFlag;

};

