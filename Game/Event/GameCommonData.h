#pragma once
#include"../KazLibrary/Helper/ISinglton.h"

class GameCommonData:public ISingleton<GameCommonData>
{
public:
	GameCommonData()
	{
		IsSecondFlag = false;
	}
	//���ɃQ�[�����N���������ǂ���
	bool IsSecondFlag;

};

