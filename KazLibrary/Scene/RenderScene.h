#pragma once
#include"../KazLibrary/Scene/SceneBase.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Render/DrawingByRasterize.h"
#include"../KazLibrary/Render/BoxPolygonRender.h"
#include"../Game/Helper/CameraWork.h"

class RenderScene :public SceneBase
{
public:
	RenderScene();
	~RenderScene();

	void Init();
	void PreInit();
	void Finalize();
	void Input();
	void Update();
	void Draw();

	int SceneChange();

private:
	DrawingByRasterize rasterizeRenderer;

	std::array<std::unique_ptr<DrawFunc::KazRender>, 3> testRArray;
	std::array<KazMath::Transform3D, 3> transformArray;
	std::array<KazMath::Color, 3> colorArray;
	DirectX::XMMATRIX motherMat;

	BoxPolygonRender boxR;

	CameraWork camera;

	std::array<std::unique_ptr<DrawExcuteIndirect>, 2> gpuParticleRender;
	std::array<Microsoft::WRL::ComPtr<ID3D12RootSignature>, 2> rootSignatureArray;



	std::unique_ptr<KazRenderHelper::ID3D12ResourceWrapper> vertexBuffer, indexBuffer;
	KazRenderHelper::ID3D12ResourceWrapper gpuVertexBuffer, gpuIndexBuffer;

	KazRenderHelper::ID3D12ResourceWrapper uavMatBuffer;


	void GenerateRect(std::array<Vertex, 4>&lVerticesArray, std::array<USHORT, 6> &lIndicesArray)
	{
		lIndicesArray = KazRenderHelper::InitIndciesForPlanePolygon();
		KazRenderHelper::InitVerticesPos(&lVerticesArray[0].pos, &lVerticesArray[1].pos, &lVerticesArray[2].pos, &lVerticesArray[3].pos, { 0.5f,0.5f });
		KazRenderHelper::InitUvPos(&lVerticesArray[0].uv, &lVerticesArray[1].uv, &lVerticesArray[2].uv, &lVerticesArray[3].uv);

		BUFFER_SIZE lVertBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lVerticesArray.size(), sizeof(Vertex));
		BUFFER_SIZE lIndexBuffSize = KazBufferHelper::GetBufferSize<BUFFER_SIZE>(lIndicesArray.size(), sizeof(UINT));


		vertexBuffer = std::make_unique<KazRenderHelper::ID3D12ResourceWrapper>();
		indexBuffer = std::make_unique<KazRenderHelper::ID3D12ResourceWrapper>();


		vertexBuffer->CreateBuffer(KazBufferHelper::SetVertexBufferData(lVertBuffSize));
		indexBuffer->CreateBuffer(KazBufferHelper::SetIndexBufferData(lIndexBuffSize));

		gpuVertexBuffer.CreateBuffer(KazBufferHelper::SetGPUBufferData(lVertBuffSize));
		gpuIndexBuffer.CreateBuffer(KazBufferHelper::SetGPUBufferData(lIndexBuffSize));

		vertexBuffer->TransData(lVerticesArray.data(), lVertBuffSize);
		indexBuffer->TransData(lIndicesArray.data(), lIndexBuffSize);

		gpuVertexBuffer.CopyBuffer(vertexBuffer->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
		gpuIndexBuffer.CopyBuffer(indexBuffer->GetBuffer(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);
	}

	struct OutputData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMFLOAT4 color;
	};
};

