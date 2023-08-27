#include "EnemyDissolveParam.h"

void EnemyDissolveParam::Setting()
{
	m_dissolveConstBuffer = KazBufferHelper::SetConstBufferData(sizeof(DissolveParam));
	for (auto& index : m_dissolve.m_facter) {
		index = 0.0f;
	}
	m_dissolveConstBuffer.bufferWrapper->TransData(&m_dissolve, sizeof(DissolveParam));
}

void EnemyDissolveParam::Update()
{
	m_dissolveConstBuffer.bufferWrapper->TransData(&m_dissolve, sizeof(DissolveParam));
}
