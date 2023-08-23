#include "ShaderRandomTable.h"
#include"../KazLibrary/Math/KazMath.h"

ShaderRandomTable::ShaderRandomTable()
{
	{
		m_shaderRandomTableBuffer = KazBufferHelper::SetUploadBufferData(sizeof(UINT) * RANDOM_TABLE_NUM, "RandomTable-UAV-UploadBuffer");
		std::array<UINT, RANDOM_TABLE_NUM>table;
		for (int i = 0; i < RANDOM_TABLE_NUM; ++i)
		{
			table[i] = KazMath::Rand<UINT>(50000000, 0);
		}
		m_shaderRandomTableBuffer.bufferWrapper->TransData(table.data(), sizeof(UINT) * RANDOM_TABLE_NUM);
		m_shaderRandomTableBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		m_shaderRandomTableBuffer.structureSize = sizeof(UINT);
		m_shaderRandomTableBuffer.elementNum = RANDOM_TABLE_NUM;
	}

	{
		m_shaderRandomTableForCurlNoizeBuffer = KazBufferHelper::SetUploadBufferData(sizeof(UINT) * RANDOM_TABLE_NUM, "RandomTable-UAV-UploadBuffer");
		std::array<UINT, RANDOM_TABLE_NUM>table;
		for (int i = 0; i < RANDOM_TABLE_NUM; ++i)
		{
			table[i] = KazMath::Rand<UINT>(1024, 0);
		}
		m_shaderRandomTableForCurlNoizeBuffer.bufferWrapper->TransData(table.data(), sizeof(UINT) * RANDOM_TABLE_NUM);
		m_shaderRandomTableForCurlNoizeBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		m_shaderRandomTableForCurlNoizeBuffer.structureSize = sizeof(UINT);
		m_shaderRandomTableForCurlNoizeBuffer.elementNum = RANDOM_TABLE_NUM;
	}
}

const KazBufferHelper::BufferData& ShaderRandomTable::GetBuffer(GraphicsRootParamType arg_rootparam)
{
	m_shaderRandomTableBuffer.rootParamType = arg_rootparam;
	return m_shaderRandomTableBuffer;
}

const KazBufferHelper::BufferData& ShaderRandomTable::GetCurlBuffer(GraphicsRootParamType arg_rootparam)
{
	m_shaderRandomTableForCurlNoizeBuffer.rootParamType = arg_rootparam;
	return m_shaderRandomTableForCurlNoizeBuffer;
}
