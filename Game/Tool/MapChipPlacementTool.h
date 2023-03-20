#pragma once
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/Collision/CollisionManager.h"
#include"../Game/Debug/ParameterMgr.h"
#include<string>

/// <summary>
/// 3D空間上のブロックを配置するツール
/// </summary>
class MapChipPlacementTool
{
	static const int BLOCK_MAX_NUM = 30000;
	static const KazMath::Vec3<float> MAP_SIZE;
	typedef std::array<KazMath::Vec3<float>, 200> Z_ARRAY;
	typedef std::array<Z_ARRAY, 30> Y_ARRAY;
	typedef std::array<Y_ARRAY, 100> POS_ARRAY;

public:
	MapChipPlacementTool();
	void Init();
	void Input(bool PUT_FLAG, bool RELEASE_FLAG, const DirectX::XMFLOAT2 &MOUSE_POS);
	void Update();
	void Draw();

	const POS_ARRAY &GetPosArray()
	{
		return blockPosArray;
	};

	bool isLoadFlag;

private:
	POS_ARRAY blockPosArray;

	static const int xSize = 300;
	static const int ySize = 200;
	static const int matSize = 30000;
	const float BLOCK_SIZE;
	Ray ray;

	//UI
	BoxPolygonRender selectR;//何処を選択しているか
	KazMath::Vec3<int> selectingIndex;
	KazMath::Vec3<int> putIndex;
	KazMath::Vec3<int> startBlockPutInOnceSize;
	KazMath::Vec3<int> endBlockPutInOnceSize;

	//判定
	enum MESH_DIR
	{
		NONE = -1,
		TOP,
		BUTTOM,
		FRONT,
		BACK,
		LEFT,
		RIGHT,
	};
	MESH_DIR CheckBlock(const KazMath::Vec3<float> &POS);
	bool inputFlag;
	bool releaseFlag;

	KazMath::Vec3<int> AdjIndex(MESH_DIR DIR)
	{
		switch (DIR)
		{
		case MapChipPlacementTool::TOP:
			return KazMath::Vec3<int>(0, 1, 0);
			break;
		case MapChipPlacementTool::BUTTOM:
			return KazMath::Vec3<int>(0, -1, 0);
			break;
		case MapChipPlacementTool::FRONT:
			return KazMath::Vec3<int>(0, 0, -1);
			break;
		case MapChipPlacementTool::BACK:
			return KazMath::Vec3<int>(0, 0, 1);
			break;
		case MapChipPlacementTool::LEFT:
			return KazMath::Vec3<int>(-1, 0, 0);
			break;
		case MapChipPlacementTool::RIGHT:
			return KazMath::Vec3<int>(1, 0, 0);
			break;
		default:
			break;
		}
		return KazMath::Vec3<int>(0, 0, 0);
	}



	struct MatData
	{
		DirectX::XMMATRIX mat;
	};
	BoxPolygonRenderPtr instanceFlameRender;
	ObjModelRenderPtr instanceObjRender;
	RESOURCE_HANDLE instanceHandle;
	RESOURCE_HANDLE instanceBoxHandle;


	std::array<MatData, matSize>lMatData;
	std::array<MatData, BLOCK_MAX_NUM>blockMatData;
	int matIndex = 0;

	Sprite2DRender cursorR;

	const float REV_VALUE = -10000.0f;
	const float CONTINE_VALUE = -5000.0f;


	//ブロックの座標をファイルに書き込む処理---------------
	ParameterMgr blockFileMgr;
	std::string objectName;
	std::string filePass;
	//ブロックの座標をファイルに書き込む処理---------------

	KazMath::Vec3<int>GetBlockIndex(const KazMath::Vec3<int> &INDEX)
	{
		KazMath::Vec3<float>basePos = blockPosArray[INDEX.x][INDEX.y][INDEX.z];
		KazMath::Vec3<float>adjPos(basePos.x - BLOCK_SIZE * INDEX.x, basePos.y - BLOCK_SIZE * INDEX.y, basePos.z - BLOCK_SIZE * INDEX.z);

		if (adjPos.x <= 0.0f)
		{
			adjPos.x = 0.0f;
		}
		if (adjPos.y <= 0.0f)
		{
			adjPos.y = 0.0f;
		}
		if (adjPos.z <= 0.0f)
		{
			adjPos.z = 0.0f;
		}
		KazMath::Vec3<int> lNum(static_cast<int>(adjPos.x / BLOCK_SIZE), static_cast<int>(adjPos.y / BLOCK_SIZE), static_cast<int>(adjPos.z / BLOCK_SIZE));
		return lNum;
	}



	std::array<BoxPolygonRender, 8>debugBox;
};