#pragma once
#include"../KazLibrary/Helper/ISinglton.h"

class TimeZone :public ISingleton<TimeZone>
{

public:

	int m_timeZone;
	bool m_isSkyEffect;

	int m_skyEffefctTimer;
	const int SKY_EFFECT_TIMER = 180;

	void KillButterfly() {
		m_skyEffefctTimer = SKY_EFFECT_TIMER;
	}

	void Update() {

		if (0 < m_skyEffefctTimer) {

			--m_skyEffefctTimer;
			if (m_skyEffefctTimer == 0) {
				m_isSkyEffect = false;
				m_timeZone = 0;
			}
			else {
				m_isSkyEffect = true;
			}

		}

	}

};