#include "CreateMeshBuffer.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

CreateMeshBuffer::CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV)
{
	//���_������������--------------------------------------------
	if (VERT.size() != 0)
	{
		GenerateBuffer(DATA_VERT, GRAPHICS_PRAMTYPE_DATA, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)), VERT.data(), "MeshParticle-VERTEX-");
	}
	//���_������������--------------------------------------------

	//UV������������--------------------------------------------
	if (UV.size() != 0)
	{
		GenerateBuffer(DATA_UV, GRAPHICS_PRAMTYPE_DATA2, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)), UV.data(), "MeshParticle-UV-");
	}
	//UV������������--------------------------------------------

	UploadToVRAM();
}

CreateMeshBuffer::CreateMeshBuffer(std::vector<KazMath::Vec3<float>> VERT, std::vector<KazMath::Vec2<float>> UV)
{
	//���_������������--------------------------------------------
	if (VERT.size() != 0)
	{
		GenerateBuffer(DATA_VERT, GRAPHICS_PRAMTYPE_DATA, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)), VERT.data(), "MeshParticle-VERTEX-");
	}
	//���_������������--------------------------------------------

	//UV������������--------------------------------------------
	if (UV.size() != 0)
	{
		GenerateBuffer(DATA_UV, GRAPHICS_PRAMTYPE_DATA2, KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)), UV.data(), "MeshParticle-UV-");
	}
	//UV������������--------------------------------------------

	UploadToVRAM();
}

const KazBufferHelper::BufferData& CreateMeshBuffer::GetBufferData(MeshBufferView ENUM_VIEW)
{
	RESOURCE_HANDLE lHandle = static_cast<RESOURCE_HANDLE>(ENUM_VIEW);
	if (m_VRAMBufferHandleDataArray[lHandle].bufferWrapper)
	{
		return m_VRAMBufferHandleDataArray[lHandle];
	}
	else
	{
		//��������Ă��Ȃ��o�b�t�@�ɃA�N�Z�X���悤�Ƃ��Ă��܂��B
		assert(0);
		return m_VRAMBufferHandleDataArray[lHandle];
	}
}

void CreateMeshBuffer::GenerateBuffer(MeshBufferView TYPE, GraphicsRootParamType ROOT_TYPE, BUFFER_SIZE DATA_SIZE, void* ADDRESS, std::string BUFFER_NAME)
{
	m_uploadBufferHandleDataArray[TYPE] = KazBufferHelper::SetUploadBufferData(DATA_SIZE, BUFFER_NAME + std::string("-RAM"));
	m_uploadBufferHandleDataArray[TYPE].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_uploadBufferHandleDataArray[TYPE].rootParamType = ROOT_TYPE;
	m_uploadBufferHandleDataArray[TYPE].bufferWrapper->TransData(ADDRESS, DATA_SIZE);

	m_VRAMBufferHandleDataArray[TYPE] = KazBufferHelper::SetGPUBufferData(DATA_SIZE, BUFFER_NAME + std::string("-VRAM"));
}

void CreateMeshBuffer::UploadToVRAM()
{
	for (int i = 0; i < DATA_NORMAL; ++i)
	{
		m_VRAMBufferHandleDataArray[i].bufferWrapper->CopyBuffer(
			m_uploadBufferHandleDataArray[i].bufferWrapper->GetBuffer()
		);
		m_VRAMBufferHandleDataArray[i].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		m_VRAMBufferHandleDataArray[i].rootParamType = static_cast<GraphicsRootParamType>(GRAPHICS_PRAMTYPE_DATA + i);

		m_VRAMBufferHandleDataArray[i].bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}
