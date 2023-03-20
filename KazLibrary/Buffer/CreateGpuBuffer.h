#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/HandleMaker.h"
#include"../Helper/KazBufferHelper.h"

/// <summary>
/// バッファとディスクリプタヒープを生成するクラスです
/// </summary>
class CreateGpuBuffer
{
public:	
	CreateGpuBuffer();
	~CreateGpuBuffer();

	/// <summary>
	/// デスクリプタヒープを生成します
	/// </summary>
	/// <param name="DESCRIPTOR_NUM_MAX">生成するバッファの最大数</param>
	void CreateDescriptorHeap(unsigned int DESCRIPTOR_NUM_MAX);

	/// <summary>
	/// バッファを生成する
	/// </summary>
	/// <param name="HEAP_PROPERTIES">プロパティ</param>
	/// <param name="RESOURCE"></param>
	/// <param name="RESOURCE_STATE"></param>
	/// <param name="CLEAR_VALUE"></param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE CreateBuffer(D3D12_HEAP_PROPERTIES HEAP_PROPERTIES, D3D12_RESOURCE_DESC RESOURCE, D3D12_RESOURCE_STATES RESOURCE_STATE, D3D12_CLEAR_VALUE *CLEAR_VALUE = nullptr);


	RESOURCE_HANDLE CreateBuffer(const KazBufferHelper::BufferResourceData &BUFFER_OPTION);

	/// <summary>
	/// データをバッファに転送します
	/// </summary>
	/// <param name="HANDLE">バッファのハンドル</param>
	/// <param name="DATA">送りたいデータのアドレス</param>
	/// <param name="DATA_SIZE">送りたいデータのサイズ</param>
	void TransData(RESOURCE_HANDLE HANDLE, void *DATA, const unsigned int &DATA_SIZE);

	/// <summary>
	/// 定数バッファビューを生成します
	/// </summary>
	/// <param name="HANDLE">生成したいバッファ</param>
	/// <param name="VIEW">ビューの設定</param>
	void CreateBufferView(RESOURCE_HANDLE HANDLE, D3D12_CONSTANT_BUFFER_VIEW_DESC VIEW);

	/// <summary>
	/// シェーダーリソースビューを生成します
	/// </summary>
	/// <param name="HANDLE">生成したいバッファ</param>
	/// <param name="VIEW">ビューの設定</param>
	void CreateBufferView(RESOURCE_HANDLE HANDLE, D3D12_SHADER_RESOURCE_VIEW_DESC VIEW);

	/// <summary>
	/// バッファを開放します
	/// </summary>
	/// <param name="HANDLE">開放したいバッファのハンドル</param>
	void ReleaseBuffer(RESOURCE_HANDLE HANDLE);

	D3D12_GPU_DESCRIPTOR_HANDLE GetViewPointer(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// バッファを受け取ります
	/// </summary>
	/// <param name="HANDLE">受け取りたいバッファのハンドル</param>
	/// <returns>生成されたバッファ</returns>
	Microsoft::WRL::ComPtr<ID3D12Resource>GetBufferData(RESOURCE_HANDLE HANDLE);


	/// <summary>
	/// バッファのGPUアドレスを受け取ります
	/// </summary>
	/// <param name="HANDLE">ハンドル</param>
	/// <returns>バッファのGPUアドレス</returns>
	D3D12_GPU_VIRTUAL_ADDRESS GetGpuAddress(RESOURCE_HANDLE HANDLE);


	void *GetMapAddres(RESOURCE_HANDLE HANDLE);


	std::unique_ptr<HandleMaker> handle;
private:	
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 1000> buffers;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap;

	D3D12_CPU_DESCRIPTOR_HANDLE heapHandle;
	

	UINT increSize;
};

