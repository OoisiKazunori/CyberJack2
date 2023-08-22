#include "ShaderRandomTable.h"
#include"../KazLibrary/Math/KazMath.h"

ShaderRandomTable::ShaderRandomTable()
{
	m_shaderRandomTableBuffer = KazBufferHelper::SetUploadBufferData(sizeof(UINT) * RANDOM_TABLE_NUM, "RandomTable-UAV-UploadBuffer");
	std::array<UINT, RANDOM_TABLE_NUM>table;
	for (int i = 0; i < RANDOM_TABLE_NUM; ++i)
	{
		table[i] = KazMath::Rand<UINT>(100000, 0);
	}
	m_shaderRandomTableBuffer.bufferWrapper->TransData(table.data(), sizeof(UINT) * RANDOM_TABLE_NUM);
	m_shaderRandomTableBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_shaderRandomTableBuffer.structureSize = sizeof(UINT);
	m_shaderRandomTableBuffer.elementNum = RANDOM_TABLE_NUM;
}

const KazBufferHelper::BufferData& ShaderRandomTable::GetBuffer(GraphicsRootParamType arg_rootparam)
{
	m_shaderRandomTableBuffer.rootParamType = arg_rootparam;
	return m_shaderRandomTableBuffer;
}
