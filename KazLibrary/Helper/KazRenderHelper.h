#pragma once
#include"../DirectXCommon/Base.h"
#include"../Pipeline/GraphicsPipeLineMgr.h"
#include"../Loader/TextureResourceMgr.h"
#include"../Loader/TextureResourceMgr.h"
#include"../Camera/CameraMgr.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Math/KazMath.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"

namespace KazRenderHelper
{
	struct DrawIndexedInstancedData
	{
		UINT indexCountPerInstance;
		UINT instanceCount;
		UINT startIndexLocation;
		UINT baseVertexLocation;
		UINT startInstanceLocation;
	};

	struct DrawInstancedData
	{
		UINT vertexCountPerInstance;
		UINT instanceCount;
		UINT startVertexLocation;
		UINT startInstanceLocation;
	};

	struct IASetVertexBuffersData
	{
		UINT slot;
		UINT numViews;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		IASetVertexBuffersData()
		{};
		IASetVertexBuffersData(UINT SLOT, UINT NUM_VIEWS, D3D12_VERTEX_BUFFER_VIEW VIEW):slot(SLOT),numViews(NUM_VIEWS), vertexBufferView(VIEW)
		{};
	};

	//複数メッシュ用の頂点、インデックス情報
	struct MultipleMeshesDrawIndexInstanceCommandData
	{
		D3D_PRIMITIVE_TOPOLOGY topology;
		std::vector<KazRenderHelper::IASetVertexBuffersData> vertexBufferDrawData;
		std::vector<D3D12_INDEX_BUFFER_VIEW> indexBufferView;
		std::vector<KazRenderHelper::DrawIndexedInstancedData> drawIndexInstancedData;

		void Finalize()
		{
			vertexBufferDrawData.clear();
			indexBufferView.clear();
			drawIndexInstancedData.clear();
		}
	};

	struct DrawIndexInstanceCommandData
	{
		D3D_PRIMITIVE_TOPOLOGY topology;
		KazRenderHelper::IASetVertexBuffersData vertexBufferDrawData;
		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		KazRenderHelper::DrawIndexedInstancedData drawIndexInstancedData;
	};

	struct DrawInstanceCommandData
	{
		D3D_PRIMITIVE_TOPOLOGY topology;
		KazRenderHelper::IASetVertexBuffersData vertexBufferDrawData;
		KazRenderHelper::DrawInstancedData drawInstanceData;
	};


	MultipleMeshesDrawIndexInstanceCommandData SetMultiMeshedDrawIndexInstanceCommandData(const D3D_PRIMITIVE_TOPOLOGY &TOPOLOGY, std::vector<KazRenderHelper::IASetVertexBuffersData> VERTEX_BUFFER_DATA, std::vector<D3D12_INDEX_BUFFER_VIEW> INDEX_BUFFER_VIEW_ARRAY, std::vector<KazRenderHelper::DrawIndexedInstancedData> DRAW_INDEX_DATA);
	DrawIndexInstanceCommandData SetDrawIndexInstanceCommandData(const D3D_PRIMITIVE_TOPOLOGY &TOPOLOGY, const D3D12_VERTEX_BUFFER_VIEW &VERTEX_VIEW, const D3D12_INDEX_BUFFER_VIEW &INDEX_VIEW, UINT INDECIES_NUM, UINT INSTANCE_NUM);
	DrawInstanceCommandData SetDrawInstanceCommandData(const D3D_PRIMITIVE_TOPOLOGY &TOPOLOGY, const D3D12_VERTEX_BUFFER_VIEW &VERTEX_VIEW, UINT VERTEX_NUM, UINT INSTANCE_NUM);

	/// <summary>
	/// 描画に必要なクラスのポインタ
	/// </summary>
	struct RenderInstancesData
	{
		TextureResourceMgr *shaderResourceMgrInstance;
		CameraMgr *cameraMgrInstance;
		DirectX12Device *deviceInstance;
		DirectX12CmdList *cmdListInstance;
		GraphicsPipeLineMgr *pipelineMgr;

		RenderInstancesData() :
			shaderResourceMgrInstance(TextureResourceMgr::Instance()),
			cameraMgrInstance(CameraMgr::Instance()),
			deviceInstance(DirectX12Device::Instance()),
			cmdListInstance(DirectX12CmdList::Instance()),
			pipelineMgr(GraphicsPipeLineMgr::Instance())
		{
		}
	};

	std::array<KazMath::Vec2<float>, 4> ChangePlaneScale(const KazMath::Vec2<float> &LEFTUP_POS, const KazMath::Vec2<float> &RIGHTDOWN_POS, const KazMath::Vec2<float> &SCALE, const KazMath::Vec2<float> &ANCHOR_POINT, const KazMath::Vec2<int> &TEX_SIZE);
	std::array<KazMath::Vec2<float>, 4> ChangeModiPlaneScale(const KazMath::Vec2<float> &LEFTUP_POS, const KazMath::Vec2<float> &RIGHTDOWN_POS, const DirectX::XMFLOAT4 &SCALE, const DirectX::XMFLOAT2 &ANCHOR_POINT, const DirectX::XMFLOAT2 &TEX_SIZE);

	void FlipXUv(DirectX::XMFLOAT2 *UV_LEFTUP_POS, DirectX::XMFLOAT2 *UV_LEFTDOWN_POS, DirectX::XMFLOAT2 *UV_RIGHTUP_POS, DirectX::XMFLOAT2 *UV_RIGHTDOWN_POS);
	void FlipYUv(DirectX::XMFLOAT2 *UV_LEFTUP_POS, DirectX::XMFLOAT2 *UV_LEFTDOWN_POS, DirectX::XMFLOAT2 *UV_RIGHTUP_POS, DirectX::XMFLOAT2 *UV_RIGHTDOWN_POS);


	void VerticesCut(const KazMath::Vec2<int> &DIV_SIZE, const KazMath::Vec2<int> &DIV_LEFTUP_POS, DirectX::XMFLOAT3 *LEFTUP_POS, DirectX::XMFLOAT3 *LEFTDOWN_POS, DirectX::XMFLOAT3 *RIGHTUP_POS, DirectX::XMFLOAT3 *RIGHTDOWN_POS, const KazMath::Vec2<float> &SCALE, const KazMath::Vec2<float> &ANCHOR_POINT);
	void UVCut(const KazMath::Vec2<int> &UV_DIV_LEFTUP_POS, const KazMath::Vec2<int> &DIV_SIZE, const KazMath::Vec2<int> &TEX_SIZE, DirectX::XMFLOAT2 *LEFTUP_POS, DirectX::XMFLOAT2 *LEFTDOWN_POS, DirectX::XMFLOAT2 *RIGHTUP_POS, DirectX::XMFLOAT2 *RIGHTDOWN_POS);


	void InitUvPos(DirectX::XMFLOAT2 *UV_LEFTUP_POS, DirectX::XMFLOAT2 *UV_LEFTDOWN_POS, DirectX::XMFLOAT2 *UV_RIGHTUP_POS, DirectX::XMFLOAT2 *UV_RIGHTDOWN_POS);
	void InitVerticesPos(DirectX::XMFLOAT3 *LEFTUP_POS, DirectX::XMFLOAT3 *LEFTDOWN_POS, DirectX::XMFLOAT3 *RIGHTUP_POS, DirectX::XMFLOAT3 *RIGHTDOWN_POS, const DirectX::XMFLOAT2 &ANCHOR_POINT);

	std::array<unsigned short, 6> InitIndciesForPlanePolygon();


	int SetBufferOnCmdList(const GraphicsRootSignatureParameter &PARAM, const GraphicsRangeType &RANGE, const GraphicsRootParamType &TYPE);
	int SetBufferOnCmdList(const std::vector<RootSignatureParameter> &PARAM, const GraphicsRangeType &RANGE, const GraphicsRootParamType &TYPE);

	DirectX::XMFLOAT4 SendColorDataToGPU(DirectX::XMFLOAT4 COLOR_DATA);
};