#include "TimeZone.h"

void TimeZone::Update()
{
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
