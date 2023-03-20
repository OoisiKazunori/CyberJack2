#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../Tutorial/DirectX12MoviePlayer.h"
#include"../GameRenderTarget.h"
#include"../Game/Event/TutorialGame.h"
#include"../Game/Tutorial/Gauge.h"

/// <summary>
/// ゲーム内で流れるチュートリアル
/// </summary>
class TutorialMovie
{
public:
	TutorialMovie(bool STOP_MOVIE_FLAG);
	void Init(const std::string &TEXT,int MAX_ACHIEVEMENT_ITEMS);
	void Update();
	void Draw();

	void Play();
	void Noise();
	void Stop();

	void Achieved();
	bool End();

	void Succeed()
	{
		tutorialText.Succeed();
	}
	bool NextTutorial()
	{
		return tutorialText.NextTutorial();
	}
	RESOURCE_HANDLE GetTexture()
	{
		return outputRenderTargetHandle;
	};
private:
	//動画再生--------------------------------------------
	bool startMovieFlag;
	bool startNoiseFlag;
	bool stopFlag;
	std::unique_ptr<DirectX12MoviePlayer> moviePlayer;
	RESOURCE_HANDLE vhsSeedHandle, noiseSeedHandle;
	MovieRender movieRender;
	float seedNum;
	//動画再生--------------------------------------------

	//ノイズ再生--------------------------------------------
	Sprite2DRender normalRender;
	//ノイズ再生--------------------------------------------

	//UI--------------------------------------------
	TutorialGame tutorialText;//操作UI
	Gauge gauge;
	const int ADD_NUM;
	//UI--------------------------------------------

	RESOURCE_HANDLE renderTargetHandle;
	RESOURCE_HANDLE outputRenderTargetHandle;
	Sprite2DRender outputRender;

};