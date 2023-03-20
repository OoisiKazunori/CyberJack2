#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/HandleMaker.h"

//àÍå¬ÇµÇ©ê∂ê¨Ç≈Ç´Ç»Ç¢ÇÃÇ≈íçà”
class GraphicsDepthTest {
public:
	GraphicsDepthTest();
	~GraphicsDepthTest();

	RESOURCE_HANDLE CreateBuffer();
	void Clear(RESOURCE_HANDLE HANDLE);

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> dsvH;
private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> depthBuffer;

	HandleMaker handle;
};

