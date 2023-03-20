#include "DirectX12CmdList.h"
#include"../DirectXCommon/DirectX12Device.h"

DirectX12CmdList::DirectX12CmdList() {
	cmdList = nullptr;
	result = S_FALSE;
}

DirectX12CmdList::~DirectX12CmdList() {
	cmdList.ReleaseAndGetAddressOf();
}

void DirectX12CmdList::CreateCmdList(ID3D12CommandAllocator *cmdAllocator) {
	if (result == S_FALSE) {
		result = DirectX12Device::Instance()->dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllocator, nullptr, IID_PPV_ARGS(&cmdList));
		result = S_OK;
	}

	cmdList->SetName(L"DirectX12CmdList");

}
