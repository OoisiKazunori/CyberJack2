#include "CreateMeshBuffer.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

CreateMeshBuffer::CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV, std::vector<UINT>INDICES)
{
	//頂点情報を書き込む--------------------------------------------
	if (VERT.size() != 0)
	{
		GenerateBuffer(DATA_VERT, GRAPHICS_PRAMTYPE_DATA, VERT.size(), sizeof(DirectX::XMFLOAT3), VERT.data(), "MeshParticle-VERTEX");
	}
	//頂点情報を書き込む--------------------------------------------

	//UV情報を書き込む--------------------------------------------
	if (UV.size() != 0)
	{
		GenerateBuffer(DATA_UV, GRAPHICS_PRAMTYPE_DATA2, UV.size(), sizeof(DirectX::XMFLOAT2), UV.data(), "MeshParticle-UV");
	}
	//UV情報を書き込む--------------------------------------------

	UploadToVRAM();
}

CreateMeshBuffer::CreateMeshBuffer(std::vector<KazMath::Vec3<float>> VERT, std::vector<KazMath::Vec2<float>> UV, std::vector<UINT>INDICES)
{
	//頂点情報を書き込む--------------------------------------------
	if (VERT.size() != 0)
	{
		std::vector<KazMath::Vec3<float>> array(Convert<KazMath::Vec3<float>>(VERT, INDICES));
		if (array.size() == 0)
		{
			array = VERT;
		}
		GenerateBuffer(DATA_VERT, GRAPHICS_PRAMTYPE_DATA, array.size(), sizeof(DirectX::XMFLOAT3), array.data(), "MeshParticle-VERTEX");
	}
	//頂点情報を書き込む--------------------------------------------

	//UV情報を書き込む--------------------------------------------
	if (UV.size() != 0)
	{
		std::vector<KazMath::Vec2<float>> array(Convert<KazMath::Vec2<float>>(UV, INDICES));
		if (array.size() == 0)
		{
			array = UV;
		}
		GenerateBuffer(DATA_UV, GRAPHICS_PRAMTYPE_DATA2, array.size(), sizeof(DirectX::XMFLOAT2), array.data(), "MeshParticle-UV");
	}
	//UV情報を書き込む--------------------------------------------

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
		//生成されていないバッファにアクセスしようとしています。
		assert(0);
		return m_VRAMBufferHandleDataArray[lHandle];
	}
}

void CreateMeshBuffer::GenerateBuffer(MeshBufferView TYPE, GraphicsRootParamType ROOT_TYPE, size_t SIZE, unsigned long long STRUCTER_SIZE, void* ADDRESS, std::string BUFFER_NAME)
{
	m_uploadBufferHandleDataArray[TYPE] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<int>(SIZE,STRUCTER_SIZE), BUFFER_NAME + std::string("-RAM"));
	m_uploadBufferHandleDataArray[TYPE].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
	m_uploadBufferHandleDataArray[TYPE].rootParamType = ROOT_TYPE;
	m_uploadBufferHandleDataArray[TYPE].bufferWrapper->TransData(ADDRESS, KazBufferHelper::GetBufferSize<int>(SIZE, STRUCTER_SIZE));
	m_uploadBufferHandleDataArray[TYPE].structureSize = static_cast<UINT>(STRUCTER_SIZE);
	m_uploadBufferHandleDataArray[TYPE].elementNum = static_cast<UINT>(SIZE);

	m_VRAMBufferHandleDataArray[TYPE] = KazBufferHelper::SetGPUBufferData(KazBufferHelper::GetBufferSize<int>(SIZE, STRUCTER_SIZE), BUFFER_NAME + std::string("-VRAM"));
	m_VRAMBufferHandleDataArray[TYPE].elementNum = static_cast<UINT>(SIZE);
	m_VRAMBufferHandleDataArray[TYPE].structureSize = static_cast<UINT>(STRUCTER_SIZE);
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

		m_VRAMBufferHandleDataArray[i].bufferWrapper->ChangeBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	}
}
