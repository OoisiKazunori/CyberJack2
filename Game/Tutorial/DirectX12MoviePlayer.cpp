#include "DirectX12MoviePlayer.h"
#include"../KazLibrary/Helper/KazHelper.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Loader/MovieLoader.h"


#pragma comment(lib, "Mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfreadwrite.lib")


DirectX12MoviePlayer::DirectX12MoviePlayer()
{

	MFStartup(MF_VERSION);

	format = DXGI_FORMAT_B8G8R8A8_UNORM;

	HRESULT hr;
	hPrepare = CreateEvent(NULL, FALSE, FALSE, NULL);

	gpuBuffer = std::make_unique<CreateGpuBuffer>();
	isPlaying = false;


	Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> dxgiManager;
	UINT resetToken = 0;
	hr = MFCreateDXGIDeviceManager(&resetToken, &dxgiManager);
	dxgiManager->ResetDevice(DirectX12Device::Instance()->dev11.Get(), resetToken);

	Microsoft::WRL::ComPtr<MediaEngineNotify> notify = new MediaEngineNotify();
	notify->SetCallback(this);

	Microsoft::WRL::ComPtr<IMFAttributes> attributes;
	hr = MFCreateAttributes(&attributes, 1);
	attributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, dxgiManager.Get());
	attributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, notify.Get());
	attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, format);
	//attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);//要求された属性は無い

	//出力フォーマット指定
	attributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM);

	// Create media engine
	Microsoft::WRL::ComPtr<IMFMediaEngineClassFactory> mfFactory;
	hr = CoCreateInstance(CLSID_MFMediaEngineClassFactory,
		nullptr,
		CLSCTX_ALL,
		IID_PPV_ARGS(mfFactory.GetAddressOf()));
	// IMFMediaEngineの取得

	const DWORD flags = MF_MEDIA_ENGINE_WAITFORSTABLE_STATE;
	hr = mfFactory->CreateInstance(flags, attributes.Get(), &mediaEngine);
}

void DirectX12MoviePlayer::SetMediaSource(const std::string &fileName)
{

	//動画読み込み--------------------------------------------
	BSTR bstrURL = nullptr;
	if (bstrURL != nullptr)
	{
		CoTaskMemFree(bstrURL); bstrURL = nullptr;
	}

	std::string lFullPath = fileName;
	std::array<wchar_t, 128> wfilepat;
	KazHelper::ConvertStringToWchar_t(lFullPath, wfilepat.data(), 128);
	auto fullPath = wfilepat;
	size_t cchAllocationSize = 1 + fullPath.size();
	bstrURL = reinterpret_cast<BSTR>(::CoTaskMemAlloc(sizeof(wchar_t) * (cchAllocationSize)));
	wcscpy_s(bstrURL, cchAllocationSize, fullPath.data());
	isPlaying = false;
	isFinished = false;
	mediaEngine->SetSource(bstrURL);
	// 処理の完了を待つ.
	WaitForSingleObject(hPrepare, INFINITE);
	//動画読み込み--------------------------------------------


	//リソース準備-------------------------------------------
	DWORD lWidth, lHeight;
	mediaEngine->GetNativeVideoSize(&lWidth, &lHeight);
	width = static_cast<UINT>(lWidth);
	height = static_cast<UINT>(lHeight);

	//シェーダーリソースバッファ、ビューの用意--------------------------------------------
	MovieLoadData lData;
	lData.filePass = fileName;
	lData.x = width;
	lData.y = height;
	resourceHandle = MovieLoader::Instance()->Load(lData);
}

void DirectX12MoviePlayer::Play()
{
	if (isPlaying)
	{
		return;
	}
	if (mediaEngine)
	{
		mediaEngine->Play();
		isPlaying = true;
	}
}

void DirectX12MoviePlayer::Stop()
{
	if (!isPlaying)
	{
		return;
	}
	if (mediaEngine)
	{
		mediaEngine->Pause();
		mediaEngine->SetCurrentTime(0.0);
	}
	isPlaying = false;
	isFinished = false;
}

bool DirectX12MoviePlayer::IsEnd()
{
	return static_cast<bool>(mediaEngine->IsEnded());
}

void DirectX12MoviePlayer::Restart()
{
	mediaEngine->SetCurrentTime(0.0);
}

bool DirectX12MoviePlayer::TranferFrame()
{
	if (mediaEngine && isPlaying)
	{
		LONGLONG pts;
		HRESULT hr = mediaEngine->OnVideoStreamTick(&pts);
		if (SUCCEEDED(hr))
		{
			Microsoft::WRL::ComPtr<ID3D11Texture2D> mediaTexture;
			hr = DirectX12Device::Instance()->dev11->OpenSharedResource1(MovieLoader::Instance()->GetShareHandle(resourceHandle), IID_PPV_ARGS(mediaTexture.GetAddressOf()));
			if (SUCCEEDED(hr)) {
				MFVideoNormalizedRect rect{ 0.0f, 0.0f, 1.0f, 1.0f };
				RECT rcTarget{ 0, 0, LONG(width), LONG(height) };

				MFARGB  m_bkgColor{ 0xFF, 0xFF, 0xFF, 0xFF };
				hr = mediaEngine->TransferVideoFrame(
					mediaTexture.Get(), &rect, &rcTarget, &m_bkgColor);
				if (SUCCEEDED(hr))
				{
					return true;
				}
			}
		}
	}
	return false;
}

void DirectX12MoviePlayer::Draw()
{
	render.data.buff = MovieLoader::Instance()->GetBuffer(resourceHandle);
	render.data.handleData = resourceHandle;
	render.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	render.Draw();
}

void DirectX12MoviePlayer::OnMediaEngineEvent(uint32_t meEvent)
{
	switch (meEvent)
	{
	case MF_MEDIA_ENGINE_EVENT_LOADSTART:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_LOADSTART\n");
		break;
	case MF_MEDIA_ENGINE_EVENT_PROGRESS:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_PROGRESS\n");
		break;
	case MF_MEDIA_ENGINE_EVENT_LOADEDDATA:
		SetEvent(hPrepare);
		break;
	case MF_MEDIA_ENGINE_EVENT_PLAY:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_PLAY\n");
		break;

	case MF_MEDIA_ENGINE_EVENT_CANPLAY:
		break;

	case MF_MEDIA_ENGINE_EVENT_WAITING:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_WAITING\n");
		break;

	case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
		break;
	case MF_MEDIA_ENGINE_EVENT_ENDED:
		OutputDebugStringA("MF_MEDIA_ENGINE_EVENT_ENDED\n");
		if (mediaEngine) {
			mediaEngine->Pause();
			isPlaying = false;
			isFinished = true;
		}
		break;

	case MF_MEDIA_ENGINE_EVENT_ERROR:
		if (mediaEngine) {
			Microsoft::WRL::ComPtr<IMFMediaError> err;
			if (SUCCEEDED(mediaEngine->GetError(&err))) {
				USHORT errCode = err->GetErrorCode();
				HRESULT hr = err->GetExtendedErrorCode();
				char buf[256] = { 0 };
				sprintf_s(buf, "ERROR: Media Foundation Event Error %u (%08X)\n", errCode, static_cast<unsigned int>(hr));
				OutputDebugStringA(buf);
			}
			else
			{
				OutputDebugStringA("ERROR: Media Foundation Event Error *FAILED GetError*\n");
			}
		}
		break;
	}
}