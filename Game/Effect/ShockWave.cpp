#include "ShockWave.h"

void ShockWave::Setting()
{
	for (auto& index : m_shockWave) {
		index.m_isActive = false;
	}
	m_shockWaveParamData = KazBufferHelper::SetConstBufferData(sizeof(ShockWaveData) * 4);
	m_shockWaveParamData.bufferWrapper->TransData(m_shockWave.data(), sizeof(ShockWaveData) * 4);
}

void ShockWave::Init()
{
	for (auto& index : m_shockWave) {
		index.m_isActive = false;
	}
}

void ShockWave::Update()
{
	m_shockWaveParamData.bufferWrapper->TransData(m_shockWave.data(), sizeof(ShockWaveData) * 4);
}
