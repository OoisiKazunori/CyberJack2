#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

class StopMgr :public ISingleton<StopMgr>
{

public:

	bool m_isPause;
	bool m_isMoveOnly1F;
	int m_stopTimer;

	const int ENEMY_HIT_STOP = 3;

};