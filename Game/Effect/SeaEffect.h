#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

class SeaEffect :public ISingleton<SeaEffect>
{

public:

	//海デバッグ用
	struct DebugSeaParam
	{
		float m_freq;
		float m_amp;
		float m_choppy;
		float m_seaSpeed;
	}m_debugSeaParam;
	KazBufferHelper::BufferData m_debugSeaParamData;
	enum SEA_ID {
		CALM, NORMAL, STORMY
	};

	//海のエフェクトに使用する変数。
	bool m_isOldSeaEffect;
	bool m_isSeaEffect;
	int m_seaID;

public:

	void Setting();

	void Update();

};

