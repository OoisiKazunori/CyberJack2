#pragma once
#include"../KazLibrary/Helper/ISinglton.h"

class GameCommonData:public ISingleton<GameCommonData>
{
public:
	GameCommonData()
	{
		IsSecondFlag = false;
	}
	//Šù‚ÉƒQ[ƒ€‚ğ‹N“®‚µ‚½‚©‚Ç‚¤‚©
	bool IsSecondFlag;

};

