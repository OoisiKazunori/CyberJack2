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

/// <summary>
/// 動画公開のテスト
/// </summary>
class DirectXMoive
{
public:
	DirectXMoive();
	void Update();
	void Draw();
	void DecodeFrame();
	void UpdateTexture(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList);

	void Play()
	{
		if (isPlaying)
		{
			return;
		}

		QueryPerformanceCounter(&startedTime);
		isPlaying = true;
	}



private:
	IMFSourceReader *mfSourceReader;
	UINT width, height;
	double fps;

	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts;
	UINT64 rowPitchBytes = 0;

	struct MovieFrameInfo
	{
		double timestamp;
		int bufferIndex;
	};

	std::list<MovieFrameInfo>decorded;
	int writeBufferIndex;
	static const int DecodeBufferCount = 2;

	std::array<RESOURCE_HANDLE, DecodeBufferCount>frameDecoded;

	std::unique_ptr<CreateGpuBuffer>gpuBuffer;

	BufferMemorySize memorySize;
	RESOURCE_HANDLE movieTexHandle;
	MovieRender render;


	bool isPlaying;
	bool isFinished;
	bool isDecodeFinished;
	bool isLoop;
	LARGE_INTEGER startedTime;

};