#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Math/KazMath.h"
#include <Helper/KazBufferHelper.h>

class ShockWave :public ISingleton<ShockWave>
{

public:

	struct ShockWaveData {

		KazMath::Vec3<float> m_pos;
		float m_radius;
		float m_power;
		int m_isActive;
		float m_facter;
		float m_pad;

	};

	std::array<ShockWaveData, 8> m_shockWave;
	KazBufferHelper::BufferData m_shockWaveParamData;

	void Setting();
	void Init();
	void Update();

};