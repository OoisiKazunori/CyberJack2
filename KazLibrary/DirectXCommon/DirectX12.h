#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/OutPutDebugStringAndCheckResult.h"
#include<vector>

/// <summary>
/// DirectXの初期化を行います
/// </summary>
class DirectX12
{
public:
	DirectX12();
	~DirectX12();

	/// <summary>
	/// DirectX12の
	/// </summary>
	/// <param name="WINDOW_SIZE_X">ウィンドウのXサイズ</param>
	/// <param name="WINDOW_SIZE_Y">ウィンドウのYサイズ</param>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <returns>S_OK...成功,それ以外...失敗</returns>
	HRESULT Create(int WINDOW_SIZE_X, int WINDOW_SIZE_Y, HWND hwnd);

	/// <summary>
	/// コマンドリストの実行
	/// </summary>
	void ActCommand();


	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;

private:

	int winX, winY;



	HRESULT result;

	Microsoft::WRL::ComPtr<IDXGIFactory6> dxgiFactory;
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;


	Microsoft::WRL::ComPtr<IDXGIAdapter> tmpAdapter;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers;
	UINT bbIndex;

	D3D12_RESOURCE_BARRIER barrierDesc{};

	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal;



	//毎回生成した時に出来てるか確認できるように返り値で確認できるようにする

	HRESULT CreateAdapter();
	HRESULT CreateCommand();
	HRESULT CreateSwapChain(int WINDOW_SIZE_X, int WINDOW_SIZE_Y, HWND hwnd);
	HRESULT CreateRenderTarget();
	HRESULT CreateFence();

};

