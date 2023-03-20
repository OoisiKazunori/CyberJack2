#pragma once
#include"../DirectXCommon/Base.h"
#include"../Scene/SceneBase.h"
#include"../Render/BackGroundForDebug.h"
#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../KazLibrary/RenderTarget/GaussianBuler.h"

/// <summary>
/// パーティクルの移動系
/// </summary>
struct UpdateData
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 velocity;
	DirectX::XMFLOAT4 color;
	int timer;
};

/// <summary>
/// コマンドバッファのアドレスの参照先
/// </summary>
struct OutPutData
{
	DirectX::XMMATRIX mat;
	DirectX::XMFLOAT4 color;
};

struct CommonData
{
	DirectX::XMMATRIX cameraMat;
	DirectX::XMMATRIX projectionMat;
	DirectX::XMFLOAT4 emittPos;
	UINT increSize;
	UINT64 gpuAddress;
	int seed;
};

struct IndirectCommand
{
	D3D12_GPU_VIRTUAL_ADDRESS cbv;
	D3D12_DRAW_ARGUMENTS drawArguments;
};

struct CSRootConstants
{
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	float size;
	UINT64 gpuAddress;
};

//移動情報
struct SceneConstantBuffer
{
	DirectX::XMFLOAT4 velocity;
	DirectX::XMFLOAT4 offset;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4X4 projection;
	// Constant buffers are 256-byte aligned. Add padding in the struct to allow multiple buffers
	// to be std::array-indexed.
	float padding[36];
};

struct DrawConstantBuffer
{
	DirectX::XMFLOAT4 projection;
};

static const int INSTANCE_NUM_MAX = 800;
class DebugScene :public SceneBase
{
public:
	DebugScene();
	~DebugScene();
	void Init();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	//void Input(ControllerInputManager *INPUT);
	int SceneChange();


private:
	KazMath::Vec3<float> debugCameraMove;
	KazMath::Vec3<float> eyePos, targetPos;
	KazMath::Vec2<float> angle;



	//BackGroundForDebug bg;
	Sprite2DRender mainRender;
	Sprite2DRender lumiRender;
	Sprite2DRender addRender;
	RESOURCE_HANDLE mainHandle;
	RESOURCE_HANDLE lumiHandle;
	RESOURCE_HANDLE addHandle;


	static const int DRAW_CALL = 1;
	unique_ptr<CreateGpuBuffer>buffer;
	RESOURCE_HANDLE inputHandle, updateHandle, outputMatHandle, drawCommandHandle, counterBufferHandle;
	RESOURCE_HANDLE commonHandle, cbvMatHandle;
	void *pointer;
	GaussianBuler bulr;

	UpdateData inputData;
	BufferMemorySize cbvSize;
	BufferMemorySize computeMemSize;
	int srvHandle;
	int cbvHandle;


	static const int TRIANGLE_ARRAY_NUM = 5000;
	static const int FRAME_COUNT = 2;
	static const int TRIANGLE_RESOURCE_COUNT = TRIANGLE_ARRAY_NUM * FRAME_COUNT;
	static const int ComputeThreadBlockSize = TRIANGLE_ARRAY_NUM * FRAME_COUNT;
	static const int CommandSizePerFrame = TRIANGLE_ARRAY_NUM * sizeof(IndirectCommand);

	RESOURCE_HANDLE commandBufferHandle;
	Microsoft::WRL::ComPtr<ID3D12CommandSignature> commandSig;
	std::array<SceneConstantBuffer, TRIANGLE_ARRAY_NUM> constantBufferData;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	int seed;
	CSRootConstants rootConst;

	RESOURCE_HANDLE paramCBHandle;

	BackGroundForDebug bg;
	


	DirectX::XMMATRIX Translate(DirectX::XMFLOAT3 VECTOR)
	{
		DirectX::XMMATRIX result;
		DirectX::XMVECTOR first = { 1.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR second = { 0.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR third = { 0.0f, 0.0f, 1.0f, 0.0f };
		DirectX::XMVECTOR forth = { VECTOR.x, VECTOR.y, VECTOR.z, 1.0f };

		result.r[0] = first;
		result.r[1] = second;
		result.r[2] = third;
		result.r[3] = forth;
		return result;
	}

	DirectX::XMMATRIX Scale(DirectX::XMFLOAT3 VECTOR)
	{
		DirectX::XMMATRIX result;
		DirectX::XMVECTOR first = { VECTOR.x, 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR second = { 0.0f, VECTOR.y, 0.0f, 0.0f };
		DirectX::XMVECTOR third = { 0.0f, 0.0f, VECTOR.z, 0.0f };
		DirectX::XMVECTOR forth = { 0.0f, 0.0f, 0.0f, 1.0f };

		result.r[0] = first;
		result.r[1] = second;
		result.r[2] = third;
		result.r[3] = forth;
		return result;
	}

	DirectX::XMMATRIX RotateX(float ANGLE)
	{
		float lsin = sin(ANGLE);
		float lcos = cos(ANGLE);

		DirectX::XMMATRIX result;
		DirectX::XMVECTOR first = { 1.0f, 0.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR second = { 0.0f, lcos, lsin, 0.0f };
		DirectX::XMVECTOR third = { 0.0f, -lsin, lcos, 0.0f };
		DirectX::XMVECTOR forth = { 0.0f, 0.0f, 0.0f, 1.0f };

		result.r[0] = first;
		result.r[1] = second;
		result.r[2] = third;
		result.r[3] = forth;
		return result;
	}

	DirectX::XMMATRIX RotateY(float ANGLE)
	{
		float lsin = sin(ANGLE);
		float lcos = cos(ANGLE);

		DirectX::XMMATRIX result;
		DirectX::XMVECTOR first = { lcos, 0.0f, -lsin, 0.0f };
		DirectX::XMVECTOR second = { 0.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR third = { lsin, 0.0f, lcos, 0.0f };
		DirectX::XMVECTOR forth = { 0.0f, 0.0f, 0.0f, 1.0f };

		result.r[0] = first;
		result.r[1] = second;
		result.r[2] = third;
		result.r[3] = forth;

		return result;
	}

	DirectX::XMMATRIX RotateZ(float ANGLE)
	{
		float lsin = sin(ANGLE);
		float lcos = cos(ANGLE);

		DirectX::XMMATRIX result;
		DirectX::XMVECTOR first = { lcos, lsin, 0.0f, 0.0f };
		DirectX::XMVECTOR second = { -lsin, lcos, 0.0f, 0.0f };
		DirectX::XMVECTOR third = { 0.0f, 0.0f, 1.0f, 0.0f };
		DirectX::XMVECTOR forth = { 0.0f, 0.0f, 0.0f, 1.0f };

		result.r[0] = first;
		result.r[1] = second;
		result.r[2] = third;
		result.r[3] = forth;

		return result;
	}

	DirectX::XMMATRIX Rotate(DirectX::XMFLOAT3 ANGLE)
	{
		DirectX::XMMATRIX rota = DirectX::XMMatrixIdentity();
		rota *= RotateZ(ANGLE.z);
		rota *= RotateX(ANGLE.x);
		rota *= RotateY(ANGLE.y);
		return rota;
	}

	UINT AlignForUavCounter(UINT bufferSize)
	{
		const UINT alignment = D3D12_UAV_COUNTER_PLACEMENT_ALIGNMENT;
		return (bufferSize + (alignment - 1)) & ~(alignment - 1);
	}
};
