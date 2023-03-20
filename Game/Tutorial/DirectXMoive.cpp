#include "DirectXMoive.h"
#include"../KazLibrary/Helper/KazHelper.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"

#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")


DirectXMoive::DirectXMoive()
{
	gpuBuffer = std::make_unique<CreateGpuBuffer>();
	isPlaying = false;


	//初期化
	MFStartup(MF_VERSION);


	//IMFSourceReader インターフェースの取得-----------------

	//動画ファイルを読み込む
	std::string lFullPath = KazFilePathName::TestPath + "test.mp4";
	std::array<wchar_t, 128> wfilepat;
	KazHelper::ConvertStringToWchar_t(lFullPath, wfilepat.data(), 128);


	//フォーマットの変換に必要
	IMFAttributes *lAttribs;
	//MF_SOURCE_READER_ENABLE_VIDEO_PROCESSINGを有効化
	MFCreateAttributes(&lAttribs, 1);
	lAttribs->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

	HRESULT hr = MFCreateSourceReaderFromURL(wfilepat.data(), lAttribs, &mfSourceReader);
	if (FAILED(hr))
	{
		assert(0);
	}
	//IMFSourceReader インターフェースの取得-----------------


	//フォーマットの設定--------------------------------------------
	GUID lDesiredFormat = MFVideoFormat_RGB32;
	Microsoft::WRL::ComPtr<IMFMediaType> lMediaType;
	MFCreateMediaType(&lMediaType);
	lMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	lMediaType->SetGUID(MF_MT_SUBTYPE, lDesiredFormat);
	DWORD *lnullPtr = 0;
	mfSourceReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), lnullPtr, lMediaType.Get());
	//フォーマットの設定--------------------------------------------


	//ビデオ情報の取得--------------------------------------------
	mfSourceReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), &lMediaType);
	MFGetAttributeSize(lMediaType.Get(), MF_MT_FRAME_SIZE, &width, &height);

	UINT32 lNume, lDenom;
	MFGetAttributeRatio(lMediaType.Get(), MF_MT_FRAME_RATE, &lNume, &lDenom);

	fps = static_cast<double>(lNume) / static_cast<double>(lDenom);
	//ビデオ情報の取得--------------------------------------------


	//DX12リソース準備
	//シェーダーリソースバッファ、ビューの用意--------------------------------------------
	auto lResDescMovieTex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_B8G8R8A8_UNORM, width, height, 1, 1);
	movieTexHandle = gpuBuffer->CreateBuffer(
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		lResDescMovieTex,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		//D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
		nullptr
	);

	D3D12_SHADER_RESOURCE_VIEW_DESC lSrvDesc = {};
	lSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	lSrvDesc.Texture2D.MipLevels = 1;
	lSrvDesc.Shader4ComponentMapping = static_cast<UINT>(D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING);
	lSrvDesc.Format = lResDescMovieTex.Format;
	//m_srvMovieTexture = m_descriptorManager->Alloc(); <- Allocとは
	memorySize = DescriptorHeapMgr::Instance()->GetSize(DESCRIPTORHEAP_MEMORY_MOVIE);

	DescriptorHeapMgr::Instance()->CreateBufferView(movieTexHandle + memorySize.startSize, lSrvDesc, gpuBuffer->GetBufferData(movieTexHandle).Get());
	//シェーダーリソースバッファ、ビューの用意--------------------------------------------


	//ステージングバッファ(テクスチャ転送用バッファ)--------------------------------------------
	UINT64 lTotalBytes = 0;
	UINT lNowRows = 0;
	DirectX12Device::Instance()->dev->GetCopyableFootprints(&lResDescMovieTex, 0, 1, 0, &layouts, &lNowRows, &rowPitchBytes, &lTotalBytes);
	lTotalBytes = (std::max)(lTotalBytes, lNowRows * rowPitchBytes);
	//バッファ生成 <- フレーム分作る
	for (int i = 0; i < DecodeBufferCount; ++i)
	{
		frameDecoded[i] = gpuBuffer->CreateBuffer(KazBufferHelper::SetStructureBuffer(static_cast<BUFFER_SIZE>(lTotalBytes)));
	}
	//ステージングバッファ(テクスチャ転送用バッファ)--------------------------------------------

}

void DirectXMoive::Update()
{

	//DecodeFrame();
	//UpdateTexture(DirectX12CmdList::Instance()->cmdList);
	render.data.handleData = movieTexHandle + memorySize.startSize;
	render.data.buff = gpuBuffer->GetBufferData(movieTexHandle);

	render.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	render.data.transform.scale = { 0.5f,0.5f };
}

void DirectXMoive::DecodeFrame()
{
	if (decorded.size() == DecodeBufferCount)
	{
		return;
	}

	//デコードの処理
	//画像データを取得する
	DWORD lFlags;
	Microsoft::WRL::ComPtr<IMFSample>lSample;
	HRESULT hr = mfSourceReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), 0, NULL, &lFlags, NULL, &lSample);

	if (!SUCCEEDED(hr))
	{
		assert(0);
	}

	if (lFlags == MF_SOURCE_READERF_ENDOFSTREAM)
	{
		//ストリームが終了した
		OutputDebugStringA("SteramEnd.\n");
	}
	else
	{
		//サンプルデータからバッファを取得
		Microsoft::WRL::ComPtr<IMFMediaBuffer>lBuffer;
		DWORD lNum;
		lSample->GetBufferCount(&lNum);
		lSample->GetBufferByIndex(0, &lBuffer);

		//テクスチャへの書き込み処理
		BYTE *lSrc;
		DWORD lSize;
		UINT lSrcPitch = width * sizeof(UINT);
		lBuffer->Lock(&lSrc, NULL, &lSize);

		void *dataMap = nullptr;
		auto result = gpuBuffer->GetBufferData(frameDecoded[writeBufferIndex])->Map(0, nullptr, (void **)&dataMap);
		if (SUCCEEDED(result))
		{
			memcpy(dataMap, lSrc, lSrcPitch * height);
			gpuBuffer->GetBufferData(frameDecoded[writeBufferIndex])->Unmap(0, nullptr);
		}
		//gpuBuffer->TransData(frameDecoded[writeBufferIndex], &lSrc, lSrcPitch * height);
		lBuffer->Unlock();

		//時間情報の取得
		int64_t lTs = 0;
		int64_t lDuration = 0;
		lSample->GetSampleTime(&lTs);
		lSample->GetSampleDuration(&lDuration);

		//100ms単位 => ns単位へ
		lTs *= 100;
		lDuration *= 100;
		//double durationSec = double(lDuration) / 1000000000;
		double timestamp = double(lTs) / 1000000000;

		//表示時間と共にキューイング
		MovieFrameInfo frameInfo{};
		frameInfo.timestamp = timestamp;
		frameInfo.bufferIndex = writeBufferIndex;
		decorded.push_back(frameInfo);
		writeBufferIndex = (writeBufferIndex + 1) % DecodeBufferCount;
	}
}

void DirectXMoive::UpdateTexture(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList)
{
	if (decorded.empty())
	{
		return;
	}
	auto frameInfo = decorded.front();

	LARGE_INTEGER now, freq;
	QueryPerformanceCounter(&now);
	QueryPerformanceFrequency(&freq);
	double playTime = double(now.QuadPart - startedTime.QuadPart) / freq.QuadPart;

	// 表示してよい時間かチェック.
	if (playTime < frameInfo.timestamp) {
		//OutputDebugStringA("Stay...\n");
		return;
	}
	decorded.pop_front();

	if (isDecodeFinished && decorded.empty())
	{
		isFinished = true;
		if (isLoop)
		{
			isFinished = false;
			isDecodeFinished = false;
			QueryPerformanceCounter(&startedTime);
		}
	}


	RenderTargetStatus::Instance()->ChangeBarrier(
		gpuBuffer->GetBufferData(movieTexHandle).Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_COPY_DEST
	);

	// テクスチャの転送
	D3D12_TEXTURE_COPY_LOCATION dst{}, src{};
	dst.pResource = gpuBuffer->GetBufferData(movieTexHandle).Get();
	dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

	src.pResource = gpuBuffer->GetBufferData(frameDecoded[frameInfo.bufferIndex]).Get();
	src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	src.PlacedFootprint = layouts;
	commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

	RenderTargetStatus::Instance()->ChangeBarrier(
		gpuBuffer->GetBufferData(movieTexHandle).Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);


}

void DirectXMoive::Draw()
{
	render.Draw();
}
