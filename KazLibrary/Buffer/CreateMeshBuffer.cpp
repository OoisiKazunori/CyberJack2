#include "CreateMeshBuffer.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

CreateMeshBuffer::CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV)
{
	//���_������������--------------------------------------------
	if (VERT.size() != 0)
	{
		bufferHandleDataArray[DATA_VERT] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)), "MeshParticle-VERTEX-RAM");
		bufferHandleDataArray[DATA_VERT].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_VERT].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		bufferHandleDataArray[DATA_VERT].bufferWrapper->TransData(VERT.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)));
	}
	//���_������������--------------------------------------------

	//UV������������--------------------------------------------
	if (UV.size() != 0)
	{
		bufferHandleDataArray[DATA_UV] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)), "MeshParticle-UV-RAM");
		bufferHandleDataArray[DATA_UV].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_UV].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		bufferHandleDataArray[DATA_UV].bufferWrapper->TransData(UV.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)));
	}
	//UV������������--------------------------------------------
}

CreateMeshBuffer::CreateMeshBuffer(std::vector<KazMath::Vec3<float>> VERT, std::vector<KazMath::Vec2<float>> UV)
{
	//���_������������--------------------------------------------
	if (VERT.size() != 0)
	{
		bufferHandleDataArray[DATA_VERT] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)), "MeshParticle-VERTEX-RAM");
		bufferHandleDataArray[DATA_VERT].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_VERT].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		bufferHandleDataArray[DATA_VERT].bufferWrapper->TransData(VERT.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)));
	}
	//���_������������--------------------------------------------

	//UV������������--------------------------------------------
	if (UV.size() != 0)
	{
		bufferHandleDataArray[DATA_UV] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)), "MeshParticle-UV-RAM");
		bufferHandleDataArray[DATA_UV].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_UV].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		bufferHandleDataArray[DATA_UV].bufferWrapper->TransData(UV.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)));
	}
	//UV������������--------------------------------------------
}

const KazBufferHelper::BufferData& CreateMeshBuffer::GetBufferData(MeshBufferView ENUM_VIEW)
{
	RESOURCE_HANDLE lHandle = static_cast<RESOURCE_HANDLE>(ENUM_VIEW);
	if (bufferHandleDataArray[lHandle].bufferWrapper)
	{
		return bufferHandleDataArray[lHandle];
	}
	else
	{
		//��������Ă��Ȃ��o�b�t�@�ɃA�N�Z�X���悤�Ƃ��Ă��܂��B
		assert(0);
		return bufferHandleDataArray[lHandle];
	}
}
