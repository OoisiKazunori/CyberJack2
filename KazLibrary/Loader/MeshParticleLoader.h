#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../Game/Effect/InstanceMeshParticleData.h"
#include"../KazLibrary/Buffer/CreateMeshBuffer.h"
#include "../KazLibrary/Loader/ModelLoader.h"
#include<string>

struct MeshParticleLoadData
{
	//パーティクルの偏り
	UINT bias;
	//三角形ごとのパーティクルの数
	UINT perTriangleNum;
	//三角面の数
	UINT faceCountNum;
};

class MeshParticleLoader :public ISingleton<MeshParticleLoader>
{
public:
	MeshParticleLoader();
	const InitMeshParticleData &LoadMesh(const std::string& FIRLE_DIR, const std::string& MODEL_NAME, const DirectX::XMMATRIX *MOTHER_MAT, const MeshParticleLoadData &MESH_PARTICLE_DATA, RESOURCE_HANDLE TEX_HANDLE = -1);

private:
	HandleMaker handle;
	std::vector<CreateMeshBuffer> meshBuffer;
	std::vector<std::unique_ptr<InitMeshParticleData>> meshParticleDataArray;

	std::vector<std::string>handleNameArray;
};

