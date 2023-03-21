#include "TutorialMovie.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"

TutorialMovie::TutorialMovie(bool STOP_MOVIE_FLAG) :
	gauge(TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "Flame.png"),
		TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TutorialPath + "Gauge.png")),
	ADD_NUM(1)
{
	if (!STOP_MOVIE_FLAG)
	{
		moviePlayer = std::make_unique<DirectX12MoviePlayer>();
		moviePlayer->SetMediaSource(KazFilePathName::TutorialPath + "TutorialMovie.mp4");
	}

	renderTargetHandle = RenderTargetStatus::Instance()->CreateRenderTarget(KazMath::Vec2<UINT>(WIN_X, WIN_Y), { 0,0,0 }, DXGI_FORMAT_R8G8B8A8_UNORM);
	outputRenderTargetHandle = RenderTargetStatus::Instance()->CreateRenderTarget(KazMath::Vec2<UINT>(WIN_X, WIN_Y), { 0,0,0 }, DXGI_FORMAT_R8G8B8A8_UNORM);

	movieRender = std::make_unique<MovieRender>();

	normalRender = std::make_unique<Sprite2DRender>();
	outputRender = std::make_unique<Sprite2DRender>();

	movieRender->data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	outputRender->data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	normalRender->data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	movieRender->data.transform.scale = { 0.9f,0.9f };


	vhsSeedHandle = outputRender->CreateConstBuffer(sizeof(float), typeid(float).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	outputRender->data.pipelineName = PIPELINE_NAME_SPRITE_VHS;
	outputRender->data.handleData = renderTargetHandle;

	noiseSeedHandle = normalRender->CreateConstBuffer(sizeof(float), typeid(float).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	normalRender->data.pipelineName = PIPELINE_NAME_SPRITE_NOISE;
	normalRender->data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "NormalMovie.png");

}

TutorialMovie::~TutorialMovie()
{
	RenderTargetStatus::Instance()->DeleteRenderTarget(renderTargetHandle);
	RenderTargetStatus::Instance()->DeleteRenderTarget(outputRenderTargetHandle);
}

void TutorialMovie::Init(const std::string &TEXT, int MAX_ACHIEVEMENT_ITEMS)
{
	seedNum = 0;
	startMovieFlag = false;
	gauge.Init(MAX_ACHIEVEMENT_ITEMS);
	tutorialText.Init(TEXT);
}

void TutorialMovie::Update()
{
	//VHS‚Ì“®‰æ•`‰æ
	if (startMovieFlag && moviePlayer)
	{
		moviePlayer->Play();
		moviePlayer->TranferFrame();

		movieRender->data.buff = moviePlayer->GetBuffer();
		movieRender->data.handleData = moviePlayer->GetDescriptorHeapHandle();
	}
	else if (startNoiseFlag)
	{
		normalRender->TransData(&seedNum, noiseSeedHandle, typeid(float).name());
	}
	else
	{
		float lstopNoiseNum = -1;
		normalRender->TransData(&lstopNoiseNum, noiseSeedHandle, typeid(float).name());
	}

	seedNum += 1.0f;
	outputRender->TransData(&seedNum, vhsSeedHandle, typeid(float).name());


	if (gauge.IsMax())
	{
		tutorialText.Succeed();
	}

	if (gauge.IsMax())
	{
		tutorialText.Succeed();
	}


	tutorialText.Update();
	gauge.Update();

}

void TutorialMovie::Draw()
{
	RenderTargetStatus::Instance()->PrepareToChangeBarrier(renderTargetHandle);
	RenderTargetStatus::Instance()->ClearRenderTarget(renderTargetHandle);
	if (startMovieFlag)
	{
		tutorialText.Draw();
		gauge.Draw();
		movieRender->Draw();
	}
	else
	{
		normalRender->Draw();
	}
	RenderTargetStatus::Instance()->PrepareToChangeBarrier(outputRenderTargetHandle, renderTargetHandle);
	RenderTargetStatus::Instance()->ClearRenderTarget(outputRenderTargetHandle);
	outputRender->Draw();
	RenderTargetStatus::Instance()->PrepareToCloseBarrier(outputRenderTargetHandle);
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
}

void TutorialMovie::Play()
{
	startMovieFlag = true;
	startNoiseFlag = false;
	stopFlag = false;
}

void TutorialMovie::Noise()
{
	startMovieFlag = false;
	startNoiseFlag = true;
	stopFlag = false;
}

void TutorialMovie::Stop()
{
	startMovieFlag = false;
	startNoiseFlag = false;
	stopFlag = true;
}

void TutorialMovie::Achieved()
{
	gauge.Add(1);
}

bool TutorialMovie::End()
{
	if (!moviePlayer)
	{
		return false;
	}
	//“®‰æŽžŠÔ‚Ü‚Å“ž’B‚µ‚½‚çAÅ‰‚ÉŠª‚«–ß‚µƒgƒŠƒK[”»’è‚Åtrue‚ð•Ô‚·
	bool lIsEndFlag = moviePlayer->IsEnd();
	if (lIsEndFlag)
	{
		moviePlayer->Restart();
	}
	return lIsEndFlag;
}
