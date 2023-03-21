#include "MovieLoader.h"

MovieLoader::MovieLoader()
{
	handle.SetHandleSize(DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_MOVIE));
	buffers.handle->SetHandleSize(DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_MOVIE));
}

RESOURCE_HANDLE MovieLoader::Load(const MovieLoadData &MOVIE_DATA)
{
	for (int i = 0; i < fileNameArray.size(); ++i)
	{
		if (MOVIE_DATA.filePass == fileNameArray[i])
		{
			return DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_MOVIE).startSize + i;
		}
	}
	fileNameArray.emplace_back(MOVIE_DATA.filePass);
	RESOURCE_HANDLE lHandle = handle.GetHandle();
	RESOURCE_HANDLE lNowHandle = handle.CaluNowHandle(lHandle);


	auto format = DXGI_FORMAT_B8G8R8A8_UNORM;

	auto lResDescMovieTex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, MOVIE_DATA.x, MOVIE_DATA.y, 1, 1);

	D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS;
	// D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS は複数のキューからアクセスされることになるので追加
	D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, MOVIE_DATA.x, MOVIE_DATA.y, 1, 1);
	resDesc.Flags = flags;
	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	KazBufferHelper::BufferResourceData resourceData(heapProps, D3D12_HEAP_FLAG_SHARED, resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, "MovieBuff");
	RESOURCE_HANDLE lMovieTexHandle = buffers.CreateBuffer(resourceData);

	D3D12_SHADER_RESOURCE_VIEW_DESC lSrvDesc = {};
	lSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	lSrvDesc.Texture2D.MipLevels = 1;
	lSrvDesc.Shader4ComponentMapping = static_cast<UINT>(D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);
	lSrvDesc.Format = lResDescMovieTex.Format;

	DescriptorHeapMgr::Instance()->CreateBufferView(lHandle, lSrvDesc, buffers.GetBufferData(lMovieTexHandle).Get());



	shareHandleArray.emplace_back(HANDLE());
	DirectX12Device::Instance()->dev->CreateSharedHandle(
		buffers.GetBufferData(lMovieTexHandle).Get()
		, nullptr, GENERIC_ALL, nullptr,
		&shareHandleArray[lNowHandle]
	);

	return lHandle;
}

HANDLE MovieLoader::GetShareHandle(RESOURCE_HANDLE HANDLE)
{
	RESOURCE_HANDLE lNum = handle.CaluNowHandle(HANDLE);
	return shareHandleArray[lNum];
}

RESOURCE_HANDLE MovieLoader::GetDescriptorAddress(RESOURCE_HANDLE HANDLE)
{
	return RESOURCE_HANDLE();
}

Microsoft::WRL::ComPtr<ID3D12Resource> MovieLoader::GetBuffer(RESOURCE_HANDLE HANDLE)
{
	return buffers.GetBufferData(HANDLE).Get();
}

void MovieLoader::Relese(RESOURCE_HANDLE HANDLE)
{
	handle.DeleteHandle(HANDLE);
}
