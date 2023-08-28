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

		m_shaderRandomTableVRAMBuffer = KazBufferHelper::SetGPUBufferData(sizeof(UINT) * RANDOM_TABLE_NUM, "RandomTable-UAV-UploadBuffer");
		m_shaderRandomTableVRAMBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		m_shaderRandomTableVRAMBuffer.bufferWrapper->CopyBuffer(m_shaderRandomTableBuffer.bufferWrapper->GetBuffer());
		m_shaderRandomTableVRAMBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
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


		m_shaderRandomTableForCurlNoizeVRAMBuffer = KazBufferHelper::SetGPUBufferData(sizeof(UINT) * RANDOM_TABLE_NUM, "RandomTable-UAV-UploadBuffer");
		m_shaderRandomTableForCurlNoizeVRAMBuffer.rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		m_shaderRandomTableForCurlNoizeVRAMBuffer.bufferWrapper->CopyBuffer(m_shaderRandomTableForCurlNoizeBuffer.bufferWrapper->GetBuffer());
		m_shaderRandomTableForCurlNoizeVRAMBuffer.bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}

const KazBufferHelper::BufferData& ShaderRandomTable::GetBuffer(GraphicsRootParamType arg_rootparam)
{
	m_shaderRandomTableVRAMBuffer.rootParamType = arg_rootparam;
	return m_shaderRandomTableVRAMBuffer;
}

const KazBufferHelper::BufferData& ShaderRandomTable::GetCurlBuffer(GraphicsRootParamType arg_rootparam)
{
	m_shaderRandomTableForCurlNoizeVRAMBuffer.rootParamType = arg_rootparam;
	return m_shaderRandomTableForCurlNoizeVRAMBuffer;
}
