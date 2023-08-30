#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

class SeaEffect :public ISingleton<SeaEffect>
{

public:

	//�C�f�o�b�O�p
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

	//�C�̃G�t�F�N�g�Ɏg�p����ϐ��B
	bool m_isOldSeaEffect;
	bool m_isSeaEffect;
	int m_seaID;

public:

	void Setting();

	void Update();

};

