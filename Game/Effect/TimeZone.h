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

	void ChangeTimeZone(int arg_timeZone) {
		m_timeZone = arg_timeZone;
	}

	void InverseTimeZone() {
		m_timeZone = !m_timeZone;
	}

	int GetTimeZone() {
		return m_timeZone;
	}

	bool GetIsSkyEffect() {
		return m_isSkyEffect;
	}

	void KillButterfly() {
		m_skyEffefctTimer = SKY_EFFECT_TIMER;
	}

	void Init();

	void Update();

};