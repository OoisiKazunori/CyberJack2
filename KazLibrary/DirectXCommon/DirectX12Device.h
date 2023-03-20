#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"

#include <d3d11.h>
#include <d3d11_1.h>

/// <summary>
/// デバイスを生成、管理します
/// </summary>
class DirectX12Device:public ISingleton<DirectX12Device> 
{
public:
	DirectX12Device();
	~DirectX12Device();

	/// <summary>
	/// デバイスの生成
	/// </summary>
	/// <param name="adapter">アダプター</param>
	void CreateDevice(Microsoft::WRL::ComPtr<IDXGIAdapter> adapter);
	Microsoft::WRL::ComPtr<ID3D12Device> dev;
	Microsoft::WRL::ComPtr<ID3D11Device1>dev11;
private:
	HRESULT result;
	friend ISingleton<DirectX12Device>;

};

