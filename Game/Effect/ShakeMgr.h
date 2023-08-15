#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>
#include<algorithm>

class ShakeMgr :public ISingleton<ShakeMgr>
{

public:

	float m_shakeAmount;

	const float SUB_SHAKE = 0.2f;

	void Update() {
		m_shakeAmount = std::clamp(m_shakeAmount - SUB_SHAKE, 0.0f, 1000.0f);
	}

	KazMath::Vec3<float> GetShakeVec3() {
		return KazMath::Vec3<float>(KazMath::Rand(-m_shakeAmount, m_shakeAmount), KazMath::Rand(-m_shakeAmount, m_shakeAmount), KazMath::Rand(-m_shakeAmount, m_shakeAmount));
	}

};

