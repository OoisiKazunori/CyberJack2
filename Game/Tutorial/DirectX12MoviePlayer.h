#pragma once
#include<mfapi.h>
#include<mfidl.h>
#include<mfreadwrite.h>

#include<filesystem>
#include<compare>
#include<string>
#include <mfapi.h>
#include <mfidl.h>
#include <mfmediaengine.h>
#include <stdexcept>

#include"../KazLibrary/Buffer/CreateGpuBuffer.h"
#include"../KazLibrary/Render/MovieRender.h"

class DirectX12MoviePlayer
{
public:
	DirectX12MoviePlayer();
	void OnMediaEngineEvent(uint32_t meEvent);

	void SetMediaSource(const std::string &fileName);

	void Play();
	void Stop();
	bool IsEnd();
	void Restart();

	bool TranferFrame();

	void Draw();

	Microsoft::WRL::ComPtr<ID3D12Resource>GetBuffer()
	{
		return gpuBuffer->GetBufferData(movieTexHandle);
	};
	RESOURCE_HANDLE GetDescriptorHeapHandle()
	{
		return movieTexHandle + memorySize.startSize;
	};

private:
	bool isPlaying;
	bool isFinished;
	bool isDecodeFinished;
	bool isLoop;
	LARGE_INTEGER startedTime;

	Microsoft::WRL::ComPtr<IMFMediaEngine> mediaEngine;
	HANDLE hPrepare;
	HANDLE hSharedTexture;
	DXGI_FORMAT format;

	IMFSourceReader *mfSourceReader;
	UINT width, height;

	std::unique_ptr<CreateGpuBuffer>gpuBuffer;
	BufferMemorySize memorySize;
	RESOURCE_HANDLE movieTexHandle;
	MovieRender render;


	static int TEX_HANDLE;
};


/// <summary>
/// コールバック
/// </summary>
class MediaEngineNotify : public IMFMediaEngineNotify {
	long m_cRef;
	DirectX12MoviePlayer *m_callback;
public:
	MediaEngineNotify() : m_cRef(0), m_callback(nullptr) {
	}

	STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
	{
		if (__uuidof(IMFMediaEngineNotify) == riid) {
			*ppv = static_cast<IMFMediaEngineNotify *>(this);
		}
		else {
			*ppv = nullptr;
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&m_cRef);
	}
	STDMETHODIMP_(ULONG) Release()
	{
		LONG cRef = InterlockedDecrement(&m_cRef);
		if (cRef == 0) {
			delete this;
		}
		return cRef;
	}
	void SetCallback(DirectX12MoviePlayer *target)
	{
		m_callback = target;
	}

	STDMETHODIMP EventNotify(DWORD meEvent, DWORD_PTR param1, DWORD param2)
	{
		DWORD lNum = param2;
		lNum = 0;

		if (meEvent == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
		{
			SetEvent(reinterpret_cast<HANDLE>(param1));
		}
		else
		{
			m_callback->OnMediaEngineEvent(meEvent);
		}
		return S_OK;
	}
};
