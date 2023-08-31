#pragma once
#include"../KazLibrary/Helper/ISinglton.h"

class TimeZone :public ISingleton<TimeZone>
{

private:

	int m_timeZone;
	bool m_isSkyEffect;

	int m_skyEffefctTimer;
	const int SKY_EFFECT_TIMER = 180;

public:

	void KillButterfly() {
		m_skyEffefctTimer = SKY_EFFECT_TIMER;
	}

	void Update();

};