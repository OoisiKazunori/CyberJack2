#include "SeaEffect.h"

void SeaEffect::UpdateFlag()
{

	SeaEffect::Instance()->m_isOldSeaEffect = SeaEffect::Instance()->m_isSeaEffect;
	SeaEffect::Instance()->m_isSeaEffect = false;

}

void SeaEffect::Setting()
{

	m_seaID = SEA_ID::CALM;
	m_debugSeaParam.m_freq = 0.16f;
	m_debugSeaParam.m_amp = 0.6f;
	m_debugSeaParam.m_choppy = 4.0f;
	m_debugSeaParam.m_seaSpeed = 5.8f;
	m_debugSeaParamData = KazBufferHelper::SetConstBufferData(sizeof(DebugSeaParam));
	m_debugSeaParamData.bufferWrapper->TransData(&m_debugSeaParam, sizeof(DebugSeaParam));

	m_isSeaEffect = false;

}

void SeaEffect::Update()
{


	const float CALM_SEA_AMP = 0.02f;
	const float CALM_SEA_FREQ = 0.16f;

	const float NORMAL_SEA_AMP = 0.6f;
	const float NORMAL_SEA_FREQ = 0.16f;

	const float STORMY_SEA_AMP = 2.8f;
	const float STORMY_SEA_FREQ = 0.16f;
	const float STORMY_SEA_SPEED = 10.0f;

	const float EFFECT_FREQ = 0.01f;

	const float SEA_SPEED = 5.8f;

	//Ã‚©‚ÈŠC‚¾‚Á‚½‚ç
	float baseAmp = 0.0f;
	float baseFreq = 0.0f;
	float baseSeaSpeed = SEA_SPEED;
	if (SeaEffect::Instance()->m_seaID == SEA_ID::CALM) {
		baseAmp = CALM_SEA_AMP;
		baseFreq = CALM_SEA_FREQ;
	}
	else if (SeaEffect::Instance()->m_seaID == SEA_ID::NORMAL) {
		baseAmp = NORMAL_SEA_AMP;
		baseFreq = NORMAL_SEA_FREQ;
	}
	else if (SeaEffect::Instance()->m_seaID == SEA_ID::STORMY) {
		baseAmp = STORMY_SEA_AMP;
		baseFreq = STORMY_SEA_FREQ;
		m_debugSeaParam.m_seaSpeed = SEA_SPEED;
		//baseSeaSpeed = STORMY_SEA_SPEED;
	}

	if (!SeaEffect::Instance()->m_isOldSeaEffect && SeaEffect::Instance()->m_isSeaEffect && SeaEffect::Instance()->m_seaID == SEA_ID::CALM) {
		m_debugSeaParam.m_amp += 0.4f;
		m_debugSeaParam.m_freq += EFFECT_FREQ;
		m_debugSeaParam.m_seaSpeed += 10.0f;
	}
	m_debugSeaParam.m_amp += (baseAmp - m_debugSeaParam.m_amp) / 8.0f;
	m_debugSeaParam.m_freq += (baseFreq - m_debugSeaParam.m_freq) / 8.0f;
	m_debugSeaParam.m_seaSpeed += (baseSeaSpeed - m_debugSeaParam.m_seaSpeed) / 20.0f;

	m_debugSeaParamData.bufferWrapper->TransData(&m_debugSeaParam, sizeof(DebugSeaParam));
}
