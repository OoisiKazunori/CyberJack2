#pragma once
#include"../DirectXCommon/Base.h"
#include"../Pipeline/GraphicsDepthTest.h"
#include"../Helper/ISinglton.h"
#include"../Buffer/CreateGpuBuffer.h"
#include"../KazLibrary/Math/KazMath.h"

struct MultiRenderTargetData
{
	DirectX::XMFLOAT3 backGroundColor;
	KazMath::Vec2<UINT> graphSize;
	MultiRenderTargetData()
	{};
};

/// <summary>
/// レンダーターゲットの切り替え
/// ダブルバッファリングの処理がメインですが
/// RenderTargetクラスにて生成した変数を使用して、ClearRenderTargetViewしたりリソースの切り替えが可能です
/// </summary>
class RenderTargetStatus :public ISingleton<RenderTargetStatus>
{
public:
	RenderTargetStatus();
	~RenderTargetStatus();

	/// <summary>
	/// ダブルバッファリング用のバッファ作成
	/// </summary>
	/// <param name="SWAPCHAIN">スワップチェーン</param>
	void CreateDoubleBuffer(Microsoft::WRL::ComPtr<IDXGISwapChain4> SWAPCHAIN);

	/// <summary>
	/// ダブルバッファリング用のレンダーターゲットをクリアします
	/// </summary>
	void SetDoubleBufferFlame();


	void ClearDoubuleBuffer(DirectX::XMFLOAT3 COLOR);

	/// <summary>
	/// ダブルバッファリング用のバリア切り替え
	/// </summary>
	void SwapResourceBarrier();

	/// <summary>
	/// レンダータゲットのリソースバリアを開きます
	/// CLOSE_RENDERTARGETがnullptrの場合、ダブルバッファリング用のフレームを閉じます
	/// </summary>
	/// <param name="OPEN_RENDERTARGET">開きたいレンダーターゲットのリソースバリア</param>
	/// <param name="CLOSE_RENDERTARGET">閉じたいレンダーターゲットのリソースバリア</param>
	void PrepareToChangeBarrier(RESOURCE_HANDLE OPEN_RENDERTARGET_HANDLE, RESOURCE_HANDLE CLOSE_RENDERTARGET_HANDLE = -1);

	/// <summary>
	/// 指定のレンダーターゲットをリソースバリアを閉じます
	/// </summary>
	/// <param name="RENDERTARGET">閉じたいレンダーターゲット</param>
	void PrepareToCloseBarrier(RESOURCE_HANDLE RENDERTARGET_HANDLE);

	/// <summary>
	/// レンダーターゲットをクリアします
	/// </summary>
	/// <param name="RENDERTARGET">クリアするレンダーターゲット</param>
	void ClearRenderTarget(RESOURCE_HANDLE RENDERTARGET_HANDLE);


	RESOURCE_HANDLE CreateRenderTarget(const KazMath::Vec2<UINT> &GRAPH_SIZE, const DirectX::XMFLOAT3 &CLEAR_COLOR, const DXGI_FORMAT &FORMAT);
	std::vector<RESOURCE_HANDLE> CreateMultiRenderTarget(const std::vector<MultiRenderTargetData> &MULTIRENDER_TARGET_DATA, const DXGI_FORMAT &FORMAT);
	ID3D12Resource *GetBufferData(RESOURCE_HANDLE HANDLE)const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetViewData(RESOURCE_HANDLE HANDLE);


	void DeleteRenderTarget(RESOURCE_HANDLE HANDLE);
	void DeleteMultiRenderTarget(const std::vector<RESOURCE_HANDLE> &HANDLE);


	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> multiPassRTVHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> copyBuffer;

	GraphicsDepthTest gDepth;
	RESOURCE_HANDLE handle, handle2;

	std::unique_ptr<CreateGpuBuffer> buffers;
	int bbIndex;

	static const int RENDERTARGET_MAX_NUM = 1000;

	//レンダーターゲットのハンドルからパス数を記録した配列
	std::array<std::vector<RESOURCE_HANDLE>, RENDERTARGET_MAX_NUM>renderTargetData;

	void ChangeBarrier(ID3D12Resource *RESOURCE, const D3D12_RESOURCE_STATES &BEFORE_STATE, const D3D12_RESOURCE_STATES &AFTER_STATE);

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	Microsoft::WRL::ComPtr<IDXGISwapChain4> copySwapchain;

	static const int SWAPCHAIN_MAX_NUM = 2;
private:


	std::array<DirectX::XMFLOAT4, RENDERTARGET_MAX_NUM> clearColors;



	
	D3D12_CPU_DESCRIPTOR_HANDLE multiPassRTVHanlde;




	struct RenderTargetHandle
	{
		unsigned int renderTargetNum;
		std::vector<RESOURCE_HANDLE> handles;
		RenderTargetHandle(unsigned int RENDER_TARGET_NUM, std::vector<RESOURCE_HANDLE> FIRST_HANDLE) :renderTargetNum(RENDER_TARGET_NUM), handles(FIRST_HANDLE)
		{
		};
	};
	std::vector<RESOURCE_HANDLE> CountPass(RESOURCE_HANDLE HANDLE);
};
