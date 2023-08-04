 #include"../Loader/TextureResourceMgr.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../Helper/KazHelper.h"
#include"../Buffer/DescriptorHeapMgr.h"
#include"../Helper/KazRenderHelper.h"

const int texWidth = 256;
const int texDataCount = texWidth * texWidth;

TextureResourceMgr::TextureResourceMgr()
{
	setHandle = 0;
	IncreSize = 0;
	result = S_FALSE;

	IncreSize = DirectX12Device::Instance()->dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	handle.SetHandleSize(DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_TEXTURE_SPRITE));
}

TextureResourceMgr::~TextureResourceMgr()
{
}

void TextureResourceMgr::Release(RESOURCE_HANDLE HANDLE)
{
	RESOURCE_HANDLE elementNum = handle.CaluNowHandle(HANDLE);
	handleName[elementNum] = "";
	bufferArray[elementNum].bufferWrapper.reset();
	bufferArray[elementNum].counterWrapper.reset();
	handle.DeleteHandle(HANDLE);
	DescriptorHeapMgr::Instance()->Release(HANDLE);
}

RESOURCE_HANDLE TextureResourceMgr::LoadGraph(std::string RESOURCE)
{

	//既に作ってあるバッファと名前が被ったら被ったハンドルを返し、被らなかったら生成
	for (int i = 0; i < handleName.size(); i++)
	{
		if (handleName[i] == RESOURCE)
		{
			return i;
		}
	}

	//string型からwchar_t型に変換
	wchar_t wfilepat[128];
	wchar_t wfilepat2[128];
	MultiByteToWideChar(CP_ACP, 0, RESOURCE.c_str(), -1, wfilepat, _countof(wfilepat));


	KazHelper::ConvertStringToWchar_t(RESOURCE, wfilepat2, 128);

	//画像ロード
	HRESULT re;
	re = LoadFromWICFile(
		wfilepat,
		DirectX::WIC_FLAGS_NONE,
		&metadata,
		scratchImg
	);

	img = scratchImg.GetImage(0, 0, 0);


	//読み込めなかったらエラーを吐く
	if (CheckResult(re, RESOURCE + "のファイル読み込みに成功しました\n", RESOURCE + "のファイル読み込みに失敗しました\n") != S_OK)
	{
		return -1;
	}


	//テクスチャの名前登録
	handleName.push_back(RESOURCE);


	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);


	//ビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);


	RESOURCE_HANDLE num = handle.GetHandle();
	RESOURCE_HANDLE elementNum = handle.CaluNowHandle(num);

	if (bufferArray.size() <= num)
	{
		bufferArray.emplace_back();
		cpuBufferArray.emplace_back();
	}

	cpuBufferArray[elementNum] = KazBufferHelper::SetShaderResourceBufferData(textureDesc);

	bufferArray[elementNum] = KazBufferHelper::BufferResourceData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		textureDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		"TextureBuffer-VRAM"
	);

	DescriptorHeapMgr::Instance()->CreateBufferView(num, srvDesc, bufferArray[elementNum].bufferWrapper->GetBuffer().Get());

	cpuBufferArray[elementNum].bufferWrapper->GetBuffer()->WriteToSubresource
	(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	bufferArray[elementNum].bufferWrapper->CopyBuffer(
		cpuBufferArray[elementNum].bufferWrapper->GetBuffer(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);


	bufferArray[elementNum].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	bufferArray[elementNum].bufferWrapper->CreateViewHandle(std::vector<RESOURCE_HANDLE>({ num }));

	return num;
}

KazBufferHelper::BufferData TextureResourceMgr::LoadGraphBuffer(std::string RESOURCE)
{
	//既に作ってあるバッファと名前が被ったら被ったハンドルを返し、被らなかったら生成
	for (int i = 0; i < handleName.size(); i++)
	{
		if (handleName[i] == RESOURCE)
		{
			return bufferArray[i];
		}
	}

	//string型からwchar_t型に変換
	wchar_t wfilepat[128];
	wchar_t wfilepat2[128];
	MultiByteToWideChar(CP_ACP, 0, RESOURCE.c_str(), -1, wfilepat, _countof(wfilepat));


	KazHelper::ConvertStringToWchar_t(RESOURCE, wfilepat2, 128);

	//画像ロード
	HRESULT re;
	re = LoadFromWICFile(
		wfilepat,
		DirectX::WIC_FLAGS_NONE,
		&metadata,
		scratchImg
	);

	img = scratchImg.GetImage(0, 0, 0);


	//読み込めなかったらエラーを吐く
	if (CheckResult(re, RESOURCE + "のファイル読み込みに成功しました\n", RESOURCE + "のファイル読み込みに失敗しました\n") != S_OK)
	{
		return KazBufferHelper::BufferData();
	}


	//テクスチャの名前登録
	handleName.push_back(RESOURCE);


	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);

	//ビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = static_cast<UINT>(metadata.mipLevels);


	RESOURCE_HANDLE num = handle.GetHandle();
	RESOURCE_HANDLE elementNum = handle.CaluNowHandle(num);

	if (bufferArray.size() <= num)
	{
		bufferArray.emplace_back();
		cpuBufferArray.emplace_back();
	}

	cpuBufferArray[elementNum] = KazBufferHelper::SetShaderResourceBufferData(textureDesc);

	bufferArray[elementNum] = KazBufferHelper::BufferResourceData
	(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		textureDesc,
		D3D12_RESOURCE_STATE_COMMON,
		nullptr,
		"TextureBuffer-VRAM"
	);

	DescriptorHeapMgr::Instance()->CreateBufferView(num, srvDesc, bufferArray[elementNum].bufferWrapper->GetBuffer().Get());

	cpuBufferArray[elementNum].bufferWrapper->GetBuffer()->WriteToSubresource
	(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	bufferArray[elementNum].bufferWrapper->CopyBuffer(
		cpuBufferArray[elementNum].bufferWrapper->GetBuffer(),
		D3D12_RESOURCE_STATE_COMMON,
		D3D12_RESOURCE_STATE_COPY_DEST
	);


	bufferArray[elementNum].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
	bufferArray[elementNum].bufferWrapper->CreateViewHandle(std::vector<RESOURCE_HANDLE>({ num }));

	return bufferArray[elementNum];
}

RESOURCE_HANDLE TextureResourceMgr::LoadDivGraph(string RESOURCE, int DIV_WIDTH_NUM, int DIV_HEIGHT_NUM, int TEXSIZE_WIDTH, int TEXSIZE_HEIGHT)
{
	//既に作ってあるバッファと名前が被ったら被ったハンドルを返し、被らなかったら生成
	for (int i = 0; i < handleName.size(); i++)
	{
		if (handleName[i] == RESOURCE)
		{
			return static_cast<RESOURCE_HANDLE>(i);
		}
	}

	//string型からwchar_t型に変換
	wchar_t wfilepat[128];
	MultiByteToWideChar(CP_ACP, 0, RESOURCE.c_str(), -1, wfilepat, _countof(wfilepat));


	//画像ロード
	HRESULT re;
	re = LoadFromWICFile(
		wfilepat,
		DirectX::WIC_FLAGS_NONE,
		&metadata,
		scratchImg
	);

	img = scratchImg.GetImage(0, 0, 0);


	//読み込めなかったらエラーを吐く
	if (CheckResult(re, RESOURCE + "のファイル読み込みに成功しました\n", RESOURCE + "のファイル読み込みに失敗しました\n") != S_OK)
	{
		return -1;
	}
	//テクスチャの名前登録
	handleName.push_back(RESOURCE);

	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels
	);


	//ビュー生成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;



	RESOURCE_HANDLE lHandle = handle.GetHandle();
	RESOURCE_HANDLE elementNum = handle.CaluNowHandle(lHandle);

	if (bufferArray.size() <= lHandle)
	{
		bufferArray.emplace_back();
	}
	bufferArray[elementNum] = KazBufferHelper::SetShaderResourceBufferData(textureDesc);

	DescriptorHeapMgr::Instance()->CreateBufferView(lHandle, srvDesc, bufferArray[elementNum].bufferWrapper->GetBuffer().Get());

	bufferArray[elementNum].bufferWrapper->GetBuffer()->WriteToSubresource
	(
		0,
		nullptr,
		img->pixels,
		(UINT)img->rowPitch,
		(UINT)img->slicePitch
	);

	//画像切り取りの情報
	divData.push_back({});
	int arrayNum = static_cast<int>(divData.size()) - 1;
	divData[arrayNum].divSize = { TEXSIZE_WIDTH,TEXSIZE_HEIGHT };
	divData[arrayNum].handle = lHandle;

	for (int y = 0; y < DIV_HEIGHT_NUM; y++)
	{
		for (int x = 0; x < DIV_WIDTH_NUM; x++)
		{
			//uv切り取りに必要な左上の点
			divData[arrayNum].divLeftUp.push_back({ 0 + TEXSIZE_WIDTH * x,0 + TEXSIZE_HEIGHT * y });
		}
	}

	return lHandle;
}

D3D12_RESOURCE_DESC TextureResourceMgr::GetTextureSize(RESOURCE_HANDLE HANDLE)
{
	//ハンドルが-1、もしくはtexBuffがnullptrだった場合、0のWidthとHeightを返す
	if (HANDLE != -1)
	{
		RESOURCE_HANDLE elementNum = handle.CaluNowHandle(HANDLE);
		if (bufferArray[elementNum].bufferWrapper->GetBuffer() != nullptr)
		{
			return bufferArray[elementNum].bufferWrapper->GetBuffer()->GetDesc();
		}
		else
		{
			D3D12_RESOURCE_DESC fail;
			fail.Width = 0;
			fail.Height = 0;
			return fail;
		}
	}
	else
	{
		D3D12_RESOURCE_DESC fail;
		fail.Width = 0;
		fail.Height = 0;
		return fail;
	}
}

DivGraphData TextureResourceMgr::GetDivData(RESOURCE_HANDLE HANDLE)
{
	if (HANDLE != -1)
	{
		for (int i = 0; i < divData.size(); i++)
		{
			if (divData[i].handle == HANDLE)
			{
				//成功
				return divData[i];
			}
		}
	}

	//失敗
	DivGraphData no;
	no.divSize = { -1,-1 };
	return no;
}

void TextureResourceMgr::SetSRV(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE)
{
	if (GRAPH_HANDLE != -1)
	{
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV = DescriptorHeapMgr::Instance()->GetGpuDescriptorView(GRAPH_HANDLE);
		int param = KazRenderHelper::SetBufferOnCmdList(PARAM, GRAPHICS_RANGE_TYPE_SRV_DESC, TYPE);
		DirectX12CmdList::Instance()->cmdList->SetGraphicsRootDescriptorTable(param, gpuDescHandleSRV);
	}
}

void TextureResourceMgr::SetSRView(RESOURCE_HANDLE GRAPH_HANDLE, GraphicsRootSignatureParameter PARAM, GraphicsRootParamType TYPE)
{
	if (GRAPH_HANDLE != -1)
	{
		int param = KazRenderHelper::SetBufferOnCmdList(PARAM, GRAPHICS_RANGE_TYPE_SRV_DESC, TYPE);
		RESOURCE_HANDLE elementNum = handle.CaluNowHandle(GRAPH_HANDLE);
		DirectX12CmdList::Instance()->cmdList->SetGraphicsRootShaderResourceView(param, bufferArray[elementNum].bufferWrapper->GetGpuAddress());
	}
}