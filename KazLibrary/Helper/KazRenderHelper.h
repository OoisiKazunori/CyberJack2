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
	std::array<UINT, 6> InitIndciesForPlanePolygonUINT();

	int SetBufferOnCmdList(const GraphicsRootSignatureParameter &PARAM, const GraphicsRangeType &RANGE, const GraphicsRootParamType &TYPE);
	int SetBufferOnCmdList(const std::vector<RootSignatureParameter> &PARAM, const GraphicsRangeType &RANGE, const GraphicsRootParamType &TYPE);

	DirectX::XMFLOAT4 SendColorDataToGPU(DirectX::XMFLOAT4 COLOR_DATA);


	class ID3D12ResourceWrapper
	{
	public:
		ID3D12ResourceWrapper();

		/// <summary>
		/// バッファ生成
		/// </summary>
		/// <param name="BUFFER_OPTION">バッファを生成する為に必要な構造体</param>
		void CreateBuffer(const KazBufferHelper::BufferResourceData &BUFFER_OPTION);

		/// <summary>
		/// データをバッファに転送します
		/// </summary>
		/// <param name="DATA">送りたいデータのアドレス</param>
		/// <param name="DATA_SIZE">送りたいデータのサイズ</param>
		void TransData(void *DATA, const unsigned int &DATA_SIZE);
		void TransData(void *DATA, unsigned int START_DATA_SIZE, unsigned int DATA_SIZE);

		/// <summary>
		/// バッファを開放します
		/// </summary>
		/// <param name="HANDLE">開放したいバッファのハンドル</param>
		void Release();

		/// <summary>
		/// バッファのGPUアドレスを受け取ります
		/// </summary>
		/// <param name="HANDLE">ハンドル</param>
		/// <returns>バッファのGPUアドレス</returns>
		D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress();

		void *GetMapAddres(int BB_INDEX=-1)const;

		void CopyBuffer(
			const Microsoft::WRL::ComPtr<ID3D12Resource> &SRC_BUFFER,
			D3D12_RESOURCE_STATES BEFORE_STATE,
			D3D12_RESOURCE_STATES AFTER_STATE
		)const
		{
			for (int i = 0; i < buffer.size(); ++i)
			{
				DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
						BEFORE_STATE,
						AFTER_STATE
					)
				);

				DirectX12CmdList::Instance()->cmdList->CopyResource(buffer[i].Get(), SRC_BUFFER.Get());

				DirectX12CmdList::Instance()->cmdList->ResourceBarrier(
					1,
					&CD3DX12_RESOURCE_BARRIER::Transition(buffer[i].Get(),
						AFTER_STATE,
						BEFORE_STATE
					)
				);
			}
		}

		const Microsoft::WRL::ComPtr<ID3D12Resource> &GetBuffer(int INDEX = -1) const
		{
			if (INDEX == -1)
			{
				return buffer[GetIndex()];
			}
			else
			{
				return buffer[INDEX];
			}
		}

		void operator=(const ID3D12ResourceWrapper &rhs)
		{
			for (int i = 0; i < buffer.size(); ++i)
			{
				rhs.buffer[i].CopyTo(&buffer[i]);
			}
		};
		
	private:
		std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 1>buffer;
		UINT GetIndex()const
		{
			return 0;
		}
	};


};