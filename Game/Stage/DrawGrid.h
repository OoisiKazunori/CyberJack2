#pragma once
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../Game/Interface/IStage.h"
#include<array>
#include<vector>
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Event/LineFlashLight.h"

class DrawGrid
{
public:
	DrawGrid(const KazMath::Color &COLOR);
	void Init(
		bool USE_FOR_FLOOR_FLAG,
		float SPACE, float BASE_POS,
		const int *CAMERA_ID_PTR,
		bool STOP_FLAG,
		bool FLASH_FLAG,
		std::array<KazMath::Color, 2>FLASH_COLOR,
		bool APPEAR_FLAG = false
	);
	void Finalize();
	void Update(float Y_POS = -1.0f, bool USE_FLASHLINE_FLAG = true);
	void Draw();

	void Appear()
	{
		appearFlag = true;
	}

private:
	//グリッドの描画ーーー
	static const int LINE_MAX = 50;
	static const int LINE_Z_MAX = LINE_MAX / 2;
	static const int LINE_X_MAX = LINE_MAX / 2;

	std::array<LineRender, LINE_MAX> gridLineRender;
	std::array<RESOURCE_HANDLE, LINE_MAX> gridLineBloomBufferHandle;
	//std::array<, LINE_MAX> gridLineBloomBufferHandle;
	std::array<std::array<KazMath::Transform3D, 2>, LINE_Z_MAX> gridFloorZLinePos;
	std::array<std::array<KazMath::Transform3D, 2>, LINE_X_MAX> gridFloorXLinePos;

	int timer;
	int maxTimer;
	int lightEffectIndex;
	std::array<std::array<KazMath::Vec3<float>, 5>, LINE_MAX> lightEffectGridFloorLineStartPos;
	std::array<std::array<KazMath::Vec3<float>, 5>, LINE_MAX> lightEffectGridFloorLineEndPos;
	std::array<std::array<LineFlashLight, 5>, LINE_MAX / 2> lightEffectArray;
	std::array<bool, LINE_MAX> lightEffectInitFlagArray;
	//グリッドの描画ーーー

	bool reversValueFlag;
	float flashRate;

	bool floorFlag;
	void InitFloor(float SPACE, float BASE_POS);
	void InitWall(float SPACE, float BASE_POS);

	const int *cameraIndex;
	float vel;

	bool flashFlag;
	std::array<KazMath::Color, 2>flashColorArray;
	bool initFlag;

	bool appearFlag;
	int appearAlpha;

	struct BloomData
	{
		DirectX::XMFLOAT4 luminanceColor;
	};
};

