#pragma once
#include"../KazLibrary/Render/KazRender.h"

struct InitBlockMountainData
{
	KazMath::Vec3<float> centralPos;
	KazMath::Vec3<float> rotation;
	KazMath::Vec3<float> lightDir;
	float yScale;
	int xRange;
	float fogDesinty;
};

/// <summary>
/// ‰š“Ê‚Ì‚ ‚é’·•ûŒ`‚ð•`‰æ‚µ‚Ü‚·
/// </summary>
class BlockMountain
{
public:
	BlockMountain();
	void Init(const InitBlockMountainData &INIT_DATA);
	void Update();
	void Draw();

private:
	int boxMaxNum;
	RESOURCE_HANDLE instanceBufferHandle;
	RESOURCE_HANDLE objectBufferHandle;
	ObjModelRenderPtr billRender;

	struct MatData
	{
		DirectX::XMMATRIX mat;
	};

	struct ObjectData
	{
		DirectX::XMFLOAT4 fogData;
	};

	struct PosUvData
	{
		KazMath::Transform3D transform;
	};

	std::vector<PosUvData> boxDataArray;

	ObjectData objectData;
	KazMath::Vec4<float>fogColor;
};

