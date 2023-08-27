#pragma once
#include "../KazLibrary/Helper/ISinglton.h"
#include <array>
#include <memory>
#include <Math/KazMath.h>
#include <Helper/KazBufferHelper.h>

class EnemyDissolveParam : public ISingleton<EnemyDissolveParam> {

public:

	struct DissolveParam {
		std::array<float, 4> m_facter;
	}m_dissolve;
	KazBufferHelper::BufferData m_dissolveConstBuffer;

public:

	void Setting();

	void Update();

};