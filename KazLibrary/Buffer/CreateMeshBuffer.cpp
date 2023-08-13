#include "CreateMeshBuffer.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

CreateMeshBuffer::CreateMeshBuffer(std::vector<DirectX::XMFLOAT3> VERT, std::vector<DirectX::XMFLOAT2> UV)
{
	//頂点情報を書き込む--------------------------------------------
	if (VERT.size() != 0)
	{
		bufferHandleDataArray[DATA_VERT] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)), "MeshParticle-VERTEX-RAM");
		bufferHandleDataArray[DATA_VERT].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_VERT].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		bufferHandleDataArray[DATA_VERT].bufferWrapper->TransData(VERT.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)));
	}
	//頂点情報を書き込む--------------------------------------------

	//UV情報を書き込む--------------------------------------------
	if (UV.size() != 0)
	{
		bufferHandleDataArray[DATA_UV] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)), "MeshParticle-UV-RAM");
		bufferHandleDataArray[DATA_UV].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_UV].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		bufferHandleDataArray[DATA_UV].bufferWrapper->TransData(UV.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)));
	}
	//UV情報を書き込む--------------------------------------------
}

CreateMeshBuffer::CreateMeshBuffer(std::vector<KazMath::Vec3<float>> VERT, std::vector<KazMath::Vec2<float>> UV)
{
	//頂点情報を書き込む--------------------------------------------
	if (VERT.size() != 0)
	{
		bufferHandleDataArray[DATA_VERT] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)), "MeshParticle-VERTEX-RAM");
		bufferHandleDataArray[DATA_VERT].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_VERT].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		bufferHandleDataArray[DATA_VERT].bufferWrapper->TransData(VERT.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(VERT.size(), sizeof(DirectX::XMFLOAT3)));
	}
	//頂点情報を書き込む--------------------------------------------

	//UV情報を書き込む--------------------------------------------
	if (UV.size() != 0)
	{
		bufferHandleDataArray[DATA_UV] = KazBufferHelper::SetUploadBufferData(KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)), "MeshParticle-UV-RAM");
		bufferHandleDataArray[DATA_UV].rangeType = GRAPHICS_RANGE_TYPE_UAV_VIEW;
		bufferHandleDataArray[DATA_UV].rootParamType = GRAPHICS_PRAMTYPE_DATA2;
		bufferHandleDataArray[DATA_UV].bufferWrapper->TransData(UV.data(), KazBufferHelper::GetBufferSize<BUFFER_SIZE>(UV.size(), sizeof(DirectX::XMFLOAT2)));
	}
	//UV情報を書き込む--------------------------------------------
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
		//生成されていないバッファにアクセスしようとしています。
		assert(0);
		return bufferHandleDataArray[lHandle];
	}
}
