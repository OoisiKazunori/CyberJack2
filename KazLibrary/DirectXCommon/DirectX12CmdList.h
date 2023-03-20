#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../DirectXCommon/DirectX12Device.h"

/// <summary>
/// コマンドリストの生成し、管理します
/// </summary>
class DirectX12CmdList:public ISingleton<DirectX12CmdList>
{
public:
	DirectX12CmdList();
	~DirectX12CmdList();

	/// <summary>
	/// コマンドリストの生成
	/// </summary>
	/// <param name="cmdAllocator">コマンドアロケーター</param>
	void CreateCmdList(ID3D12CommandAllocator *cmdAllocator);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
private:
	HRESULT result;

	friend ISingleton<DirectX12CmdList>;
};

