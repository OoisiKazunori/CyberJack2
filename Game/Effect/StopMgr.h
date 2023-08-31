#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

class StopMgr :public ISingleton<StopMgr>
{

private:

	int m_stopTimer;

	const int ENEMY_HIT_STOP = 3;

public:

	void Init();

	void Update();

	void HitStopEnemy();

	bool IsHitStop() {
		return 0 < m_stopTimer;
	}

};