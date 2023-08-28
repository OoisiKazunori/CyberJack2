#include "SoundManager.h"
#include"../Helper/KazHelper.h"



Microsoft::WRL::ComPtr<IXAudio2> SoundManager::xAudio2{};
IXAudio2MasteringVoice* SoundManager::masterVoice{};

//XAudio2VoiceCallbackのインスタンスをグローバル変数としてインスタンスを作る
XAudio2VoiceCallback voiceCallback{};

void SoundManager::SettingSoundManager()
{
	//xaudio2のエンジンを生成
	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
}

SoundData SoundManager::SoundLoadWave(char* arg_fileName)
{

	//①ファイルオープン
	//ファイル入力ストリームのインスタンス
	ifstream file{};
	//.wavをバイナリモードで開く
	file.open(arg_fileName, ios_base::binary);
	//ファイルオープンの失敗を検出する
	assert(file.is_open());

	//②.wavの読み込みと再生
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}
	//Formatチャンクの読み込み
	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);
	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK ", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "LIST ", 4) == 0) {
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data ", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);
	//Waveファイルを閉じる
	file.close();

	//returnするための音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.volume = 1.0f;

	return soundData;

	////③サウンド再生 読み込んだ.wavファイルの情報から音声を再生する
	//WAVEFORMATEX wfex{};
	////波形フォーマットの設定
	//memcpy(&wfex, &format.fmt, sizeof(format.fmt));
	//wfex.wBitsPerSample = format.fmt.nBlockAlign * 8 / format.fmt.nChannels;
	////波形フォーマットをもとにSourceVoiceの生成
	//IXAudio2SourceVoice* pSourceVoice = nullptr;
	//result = xAudio2->CreateSourceVoice(&pSourceVoice, &wfex, 0, 2.0f, &voiceCallback);
	//if FAILED(result) {
	//	delete[] pBuffer;
	//	return;
	//}
	////再生する波形データの設定
	//XAUDIO2_BUFFER buf{};
	//buf.pAudioData = (BYTE*)pBuffer;
	//buf.pContext = pBuffer;
	//buf.Flags = XAUDIO2_END_OF_STREAM;
	//buf.AudioBytes = data.size;
	////波形データの再生
	//result = pSourceVoice->SubmitSourceBuffer(&buf);
	//result = pSourceVoice->Start();
}

void SoundManager::SoundUnload(SoundData* arg_soundData)
{
	//バッファメモリを解放
	delete[]  arg_soundData->pBuffer;

	arg_soundData->pBuffer = 0;
	arg_soundData->bufferSize = 0;
	arg_soundData->wfex = {};
}

void SoundManager::SoundPlayerWave(SoundData& arg_soundData, int arg_loopCount)
{
	HRESULT result;

	//波形フォーマットをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &arg_soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = arg_soundData.pBuffer;
	buf.AudioBytes = arg_soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	buf.LoopCount = arg_loopCount;

	pSourceVoice->SetVolume(arg_soundData.volume);

	arg_soundData.source = pSourceVoice;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();
}



//
//SoundManager::SoundManager()
//{
//	//XAudioエンジンのインスタンスを生成
//	XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
//	//マスターボイスを生成
//	xAudio2->CreateMasteringVoice(&masterVoice);
//}
//
//void SoundManager::Finalize()
//{
//}
//
//RESOURCE_HANDLE SoundManager::LoadSoundMem(std::string FILE_PASS, bool BGM_FLAG)
//{
//	//読み込み重複防止
//	for (int i = 0; i < masterSoundData.size(); i++)
//	{
//		if (FILE_PASS == masterSoundData[i].filePass)
//		{
//			return static_cast<RESOURCE_HANDLE>(i);
//		}
//	}
//
//
//	std::ifstream file;
//	file.open(FILE_PASS, std::ios_base::binary);
//	if (file.fail())
//	{
//		FailCheck(FILE_PASS + "の読み込みに失敗しました\n");
//		return -1;
//	}
//
//	//RIFF------------------------------------------
//	RiffHeader riff;
//	file.read((char *)&riff, sizeof(riff));
//
//	if (strncmp(riff.chunk.id.data(), "RIFF", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "の読み込みに失敗しました\n");
//		return -1;
//	}
//	if (strncmp(riff.type.data(), "WAVE", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "の読み込みに失敗しました\n");
//		return -1;
//	}
//	//RIFF------------------------------------------
//
//
//	//FormatChunk------------------------------------------
//	FormatChunk format = {};
//	file.read((char *)&format, sizeof(ChunkHeader));
//
//	if (strncmp(format.chunk.id.data(), "fmt ", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "の読み込みに失敗しました\n");
//		return -1;
//	}
//
//	assert(format.chunk.size <= sizeof(format.fmt));
//	file.read((char *)&format.fmt, format.chunk.size);
//	//FormatChunk------------------------------------------
//
//	//ChunkHeader------------------------------------------
//	ChunkHeader data;
//	file.read((char *)&data, sizeof(data));
//	if (strncmp(data.id.data(), "JUNK", 4) == 0)
//	{
//		file.seekg(data.size, std::ios_base::cur);
//		file.read((char *)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id.data(), "LIST ", 4) == 0)
//	{
//		file.seekg(data.size, std::ios_base::cur);
//		file.read((char *)&data, sizeof(data));
//	}
//
//	if (strncmp(data.id.data(), "data ", 4) != 0)
//	{
//		FailCheck(FILE_PASS + "の読み込みに失敗しました\n");
//		return -1;
//	}
//	//ChunkHeader------------------------------------------
//
//
//	//ファイル読み込み成功
//	RESOURCE_HANDLE lHandle = handle.GetHandle();
//	
//	//true...配列以上ならpush,false...配列内ならそのまま置き換え
//	if (masterSoundData.size() <= lHandle)
//	{
//		//masterSoundData.push_back({});
//		masterSoundData[lHandle].soundData = std::make_unique<SoundData>();
//		masterSoundData[lHandle].filePass = FILE_PASS;
//	}
//	else
//	{
//		masterSoundData[lHandle].soundData = std::make_unique<SoundData>();
//		masterSoundData[lHandle].filePass = FILE_PASS;
//	}
//
//
//
//	masterSoundData[lHandle].soundData->wfex= format.fmt;
//	masterSoundData[lHandle].soundData->bufferSize= data.size;	
//	masterSoundData[lHandle].soundData->pBuffer = new BYTE[data.size];
//	file.read(reinterpret_cast<char *>(masterSoundData[lHandle].soundData->pBuffer), data.size);
//	file.close();
//
//
//	//サウンドソース生成
//	if (BGM_FLAG)
//	{
//		xAudio2->CreateSourceVoice(&masterSoundData[lHandle].soundSorce, &masterSoundData[lHandle].soundData->wfex);
//	}
//
//
//
//	SucceedCheck(FILE_PASS + "の読み込みに成功しました\n");
//
//	return lHandle;
//}
//
//void SoundManager::PlaySoundMem(RESOURCE_HANDLE HANDLE, int VOLUME, bool LOOP_FLAG)
//{
//	if (KazHelper::IsitInAnArray(HANDLE, masterSoundData.size()))
//	{
//		if (masterSoundData[HANDLE].soundData == nullptr)
//		{
//			FailCheck("音の再生に失敗しました\n");
//			return;
//		}
//
//		//音再生
//		if (masterSoundData[HANDLE].soundSorce == nullptr)
//		{
//			HRESULT result;
//			IXAudio2SourceVoice *pSourceVoice = nullptr;
//			result = xAudio2->CreateSourceVoice(&pSourceVoice, &masterSoundData[HANDLE].soundData->wfex);
//			XAUDIO2_BUFFER buf{};
//			buf.pAudioData = masterSoundData[HANDLE].soundData->pBuffer;
//			buf.AudioBytes = masterSoundData[HANDLE].soundData->bufferSize;
//			buf.Flags = XAUDIO2_END_OF_STREAM;
//			if (LOOP_FLAG)
//			{
//				buf.LoopCount = XAUDIO2_LOOP_INFINITE;
//			}
//			else
//			{
//				buf.LoopCount = XAUDIO2_NO_LOOP_REGION;
//			}
//
//			//音量調整
//			float volumeTmp = VOLUME / 100.0f;
//			if (1 <= volumeTmp)
//			{
//				volumeTmp = 1;
//			}
//			pSourceVoice->SetVolume(volumeTmp);
//			result = pSourceVoice->SubmitSourceBuffer(&buf);
//			result = pSourceVoice->Start();
//		}
//		else
//		{
//			XAUDIO2_BUFFER buf{};
//			buf.pAudioData = masterSoundData[HANDLE].soundData->pBuffer;
//			buf.AudioBytes = masterSoundData[HANDLE].soundData->bufferSize;
//			buf.Flags = XAUDIO2_END_OF_STREAM;
//			if (LOOP_FLAG)
//			{
//				buf.LoopCount = XAUDIO2_LOOP_INFINITE;
//			}
//			else
//			{
//				buf.LoopCount = XAUDIO2_NO_LOOP_REGION;
//			}
//
//			//音量調整
//			float volumeTmp = VOLUME / 100.0f;
//			if (1 <= volumeTmp)
//			{
//				volumeTmp = 1;
//			}
//			masterSoundData[HANDLE].soundSorce->SetVolume(volumeTmp);
//			masterSoundData[HANDLE].soundSorce->SubmitSourceBuffer(&buf);
//			masterSoundData[HANDLE].soundSorce->Start();
//		}
//	}
//	else
//	{
//		return;
//	}	
//}
//
//void SoundManager::ChangeSoundMem(RESOURCE_HANDLE HANDLE, int VOLUME)
//{
//	float volumeTmp = VOLUME / 100.0f;
//	masterSoundData[HANDLE].soundSorce->SetVolume(volumeTmp);
//}
//
//void SoundManager::StopSoundMem(RESOURCE_HANDLE HANDLE)
//{
//	if (KazHelper::IsitInAnArray(HANDLE, masterSoundData.size()))
//	{
//		if (masterSoundData[HANDLE].soundData != nullptr)
//		{
//			masterSoundData[HANDLE].soundSorce->Stop();
//			masterSoundData[HANDLE].soundSorce->FlushSourceBuffers();
//			masterSoundData[HANDLE].soundData->wfex;
//		}
//	}
//}
//
//void SoundManager::ReleaseSoundMem(RESOURCE_HANDLE HANDLE)
//{
//	if (KazHelper::IsitInAnArray(HANDLE, masterSoundData.size()))
//	{
//		if (masterSoundData[HANDLE].soundData != nullptr)
//		{	
//			masterSoundData[HANDLE].soundSorce->Stop();
//			masterSoundData[HANDLE].soundSorce->DestroyVoice();			
//			//サウンドデータの削除
//			masterSoundData[HANDLE].soundData.reset();
//			masterSoundData[HANDLE].soundData.reset();
//			masterSoundData[HANDLE].soundData = nullptr;
//			//ファイルパスの削除
//			masterSoundData[HANDLE].filePass = "";
//			//ハンドルの削除
//			handle.DeleteHandle(HANDLE);
//		}
//	}
//}
