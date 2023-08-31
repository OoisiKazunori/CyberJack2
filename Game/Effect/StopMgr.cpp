#include "StopMgr.h"
#include <algorithm>

void StopMgr::Init()
{
	m_stopTimer = 0;
}

void StopMgr::Update()
{
	m_stopTimer = std::clamp(m_stopTimer - 1, 0, 10000);
}

void StopMgr::HitStopEnemy()
{
	m_stopTimer = ENEMY_HIT_STOP;
}
