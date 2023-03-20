#pragma once
#include"../DirectXCommon/Base.h"
#include<xaudio2.h>
#pragma comment(lib,"xaudio2.lib")
#include<fstream>
#include"../Helper/ISinglton.h"
#include"../Helper/HandleMaker.h"

struct ChunkHeader
{
	std::array<char,4> id;		//チャンク毎のID
	int32_t size;	//チャンクサイズ
};

struct RiffHeader
{
	ChunkHeader chunk;	//RIFF
	std::array<char,4> type;	//WAVE
};

struct FormatChunk
{
	ChunkHeader chunk;	//fmt
	WAVEFORMATEX fmt; //波形フォーマット
};

struct SoundData
{
	WAVEFORMATEX wfex;
	BYTE *pBuffer;
	unsigned int bufferSize;
};

struct Sound
{
	std::string filePass;
	std::unique_ptr<SoundData> soundData;
	IXAudio2SourceVoice* soundSorce;
};
/// <summary>
/// 音再生に関するクラスです
/// </summary>
class SoundManager :public ISingleton<SoundManager>
{
public:
	SoundManager();	

	/// <summary>
	/// 全部の音を開放します
	/// </summary>
	void Finalize();

	/// <summary>
	/// .wavの音を読み込みます
	/// </summary>
	/// <param name="FILE_PASS">ファイルパス</param>
	/// <returns>ハンドル</returns>
	RESOURCE_HANDLE LoadSoundMem(std::string FILE_PASS, bool BGM_FLAG = true);

	/// <summary>
	/// 音を再生します
	/// </summary>
	/// <param name="HANDLE">再生したい音データのハンドル</param>
	/// <param name="VOLUME">音量</param>
	/// <param name="LOOP_FLAG">音をループ再生するかしないか</param>
	void PlaySoundMem(RESOURCE_HANDLE HANDLE, int VOLUME, bool LOOP_FLAG = false);

	void ChangeSoundMem(RESOURCE_HANDLE HANDLE, int VOLUME);
	/// <summary>
	/// 音楽を止めます
	/// </summary>
	/// <param name="HANDLE">止めたい音楽データのハンドル</param>
	void StopSoundMem(RESOURCE_HANDLE HANDLE);

	/// <summary>
	/// 音データをリリースします
	/// </summary>
	/// <param name="HANDLE">リリースしたい音データのハンドル</param>
	void ReleaseSoundMem(RESOURCE_HANDLE HANDLE);


private:
	HandleMaker handle;
	std::array<Sound, 30> masterSoundData;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice *masterVoice;
};

