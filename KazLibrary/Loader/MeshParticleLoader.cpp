#include "MeshParticleLoader.h"

MeshParticleLoader::MeshParticleLoader()
{
}

const InitMeshParticleData &MeshParticleLoader::LoadMesh(const std::string& FIRLE_DIR, const std::string &MODEL_NAME, const DirectX::XMMATRIX *MOTHER_MAT, const MeshParticleLoadData &MESH_PARTICLE_DATA, RESOURCE_HANDLE TEX_HANDLE)
{
	//重複確認
	for (int i = 0; i < handleNameArray.size(); i++)
	{
		if (handleNameArray[i] == MODEL_NAME)
		{
			meshParticleDataArray[i]->triagnleData =
			{
				FbxModelResourceMgr::Instance()->GetResourceData(i)->vertNum,
				MESH_PARTICLE_DATA.bias,
				MESH_PARTICLE_DATA.perTriangleNum,
				MESH_PARTICLE_DATA.faceCountNum
			};
			meshParticleDataArray[i]->motherMat = MOTHER_MAT;
			
			return *meshParticleDataArray[i];
		}
	}

	VertexData lModelHandle = ModelLoader::Instance()->Load(FIRLE_DIR,MODEL_NAME)->modelData[0].vertexData;
	if (lModelHandle.verticesArray.size() == 0)
	{
		assert(0);
	}
	int lNowHandle = handle.GetHandle();

	//新しいモデル情報の用意
	handleNameArray.emplace_back(MODEL_NAME);
	meshBuffer.emplace_back(CreateMeshBuffer(lModelHandle.verticesArray, lModelHandle.uvArray));

	InitMeshParticleData lMeshData;
	if (meshBuffer[lNowHandle].IsDataIn(CreateMeshBuffer::DATA_VERT))
	{
		lMeshData.vertData = meshBuffer[lNowHandle].GetBufferData(CreateMeshBuffer::DATA_VERT);
	}
	if (meshBuffer[lNowHandle].IsDataIn(CreateMeshBuffer::DATA_UV))
	{
		lMeshData.uvData = meshBuffer[lNowHandle].GetBufferData(CreateMeshBuffer::DATA_UV);
	}
	lMeshData.motherMat = MOTHER_MAT;
	lMeshData.triagnleData =
	{
		static_cast<UINT>(lModelHandle.verticesArray.size()),
		MESH_PARTICLE_DATA.bias,
		MESH_PARTICLE_DATA.perTriangleNum,
		MESH_PARTICLE_DATA.faceCountNum
	};

	/*VertexData lModelHandle = ModelLoader::Instance()->Load(FIRLE_DIR, MODEL_NAME)->modelData[0].materialData;
	if (FbxModelResourceMgr::Instance()->GetResourceData(lModelHandle)->textureHandle.size() != 0)
	{
		lMeshData.textureHandle = FbxModelResourceMgr::Instance()->GetResourceData(lModelHandle)->textureHandle[0];
	}
	if (TEX_HANDLE != -1)
	{
		lMeshData.textureHandle = TEX_HANDLE;
	}*/

	meshParticleDataArray.emplace_back(std::make_unique<InitMeshParticleData>(lMeshData));

	return *meshParticleDataArray[meshParticleDataArray.size() - 1];
}
