#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../Game/Effect/InstanceMeshParticle.h"
#include"../KazLibrary/Buffer/CreateMeshBuffer.h"

struct MeshParticleLoadData
{
	UINT bias;
	UINT perTriangleNum;
	UINT faceCountNum;
};

class MeshParticleLoader :public ISingleton<MeshParticleLoader>
{
public:
	MeshParticleLoader();
	const InitMeshParticleData &Load(const std::string &MODEL_NAME, bool REV_FLAG, const DirectX::XMMATRIX *MOTHER_MAT, const MeshParticleLoadData &MESH_PARTICLE_DATA, RESOURCE_HANDLE TEX_HANDLE = -1);

private:
	HandleMaker handle;
	std::vector<CreateMeshBuffer> meshBuffer;
	std::vector<std::unique_ptr<InitMeshParticleData>> meshParticleDataArray;

	std::vector<std::string>handleNameArray;
};

