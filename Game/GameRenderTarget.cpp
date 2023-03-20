#include"GameRenderTarget.h"

GameRenderTarget::GameRenderTarget(const KazMath::Color &COLOR)
{
	std::vector<MultiRenderTargetData> lData;
	lData.push_back(MultiRenderTargetData());
	lData.push_back(MultiRenderTargetData());
	lData[0].graphSize = { WIN_X,WIN_Y };
	lData[0].backGroundColor = COLOR.ConvertColorRateToXMFLOAT3();
	lData[1].graphSize = { WIN_X,WIN_Y };
	lData[1].backGroundColor = KazMath::Color(0, 0, 0, 255).ConvertColorRateToXMFLOAT3();

	handles =
		RenderTargetStatus::Instance()->CreateMultiRenderTarget(lData, DXGI_FORMAT_R8G8B8A8_UNORM);
	mainRenderTarget.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	mainRenderTarget.data.pipelineName = PIPELINE_NAME_SPRITE_NOBLEND;
	mainRenderTarget.data.handleData = handles[0];

	luminaceTex.data.pipelineName = PIPELINE_NAME_SPRITE_LUMI;
	luminaceTex.data.handleData = handles[0];
	luminaceTex.data.addHandle.handle[0] = handles[1];
	luminaceTex.data.addHandle.paramType[0] = GRAPHICS_PRAMTYPE_TEX2;
	luminaceTex.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };

	addHandle = RenderTargetStatus::Instance()->CreateRenderTarget({ WIN_X,WIN_Y }, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DXGI_FORMAT_R8G8B8A8_UNORM);
	addRenderTarget.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };
	addRenderTarget.data.pipelineName = PIPELINE_NAME_BLOOM;
	addRenderTarget.data.addHandle.paramType[0] = GRAPHICS_PRAMTYPE_TEX;
	addRenderTarget.data.addHandle.paramType[1] = GRAPHICS_PRAMTYPE_TEX2;
	addRenderTarget.data.addHandle.paramType[2] = GRAPHICS_PRAMTYPE_TEX3;
	addRenderTarget.data.addHandle.paramType[3] = GRAPHICS_PRAMTYPE_TEX4;

	buler[0] = std::make_unique<GaussianBuler>(KazMath::Vec2<UINT>(WIN_X, WIN_Y));
	buler[1] = std::make_unique<GaussianBuler>(KazMath::Vec2<UINT>(WIN_X / 2, WIN_Y / 2));
	buler[2] = std::make_unique<GaussianBuler>(KazMath::Vec2<UINT>(WIN_X / 3, WIN_Y / 3));
	buler[3] = std::make_unique<GaussianBuler>(KazMath::Vec2<UINT>(WIN_X / 4, WIN_Y / 4));

	gameTexHandle = RenderTargetStatus::Instance()->CreateRenderTarget({ WIN_X,WIN_Y }, KazMath::Color(14, 12, 13, 255).ConvertColorRateToXMFLOAT3(), DXGI_FORMAT_R8G8B8A8_UNORM);
}

GameRenderTarget::~GameRenderTarget()
{
	RenderTargetStatus::Instance()->DeleteRenderTarget(addHandle);
	RenderTargetStatus::Instance()->DeleteMultiRenderTarget(handles);
	RenderTargetStatus::Instance()->DeleteRenderTarget(gameTexHandle);
}

void GameRenderTarget::Init()
{
}

void GameRenderTarget::Finalize()
{
	//ƒŒƒ“ƒ_[ƒ^[ƒQƒbƒg‚Ì”jŠü
	RenderTargetStatus::Instance()->DeleteRenderTarget(addHandle);
	RenderTargetStatus::Instance()->DeleteMultiRenderTarget(handles);
}

void GameRenderTarget::SetRenderTarget()
{
	RenderTargetStatus::Instance()->PrepareToChangeBarrier(handles[0]);
	RenderTargetStatus::Instance()->ClearRenderTarget(handles[0]);
	//ƒQ[ƒ€•`‰æ
}

RESOURCE_HANDLE GameRenderTarget::GetGameRenderTargetHandle()
{
	return gameTexHandle;
}

void GameRenderTarget::Draw()
{
	PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Luminance");
	RenderTargetStatus::Instance()->PrepareToChangeBarrier(addHandle, handles[0]);
	RenderTargetStatus::Instance()->ClearRenderTarget(addHandle);
	//‹P“x’Šo
	luminaceTex.Draw();
	PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

	PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "DrawMain");
	RenderTargetStatus::Instance()->PrepareToChangeBarrier(gameTexHandle, addHandle);
	RenderTargetStatus::Instance()->ClearRenderTarget(gameTexHandle);
	//ƒQ[ƒ€‰æ–Ê•`‰æ
	mainRenderTarget.Draw();

	addRenderTarget.data.handleData = addHandle;
	addRenderTarget.data.addHandle.handle[0] = buler[0]->BlurImage(addHandle, gameTexHandle);
	addRenderTarget.data.addHandle.handle[1] = buler[1]->BlurImage(addRenderTarget.data.addHandle.handle[0], gameTexHandle);
	addRenderTarget.data.addHandle.handle[2] = buler[2]->BlurImage(addRenderTarget.data.addHandle.handle[1], gameTexHandle);
	addRenderTarget.data.addHandle.handle[3] = buler[3]->BlurImage(addRenderTarget.data.addHandle.handle[2], gameTexHandle);
	addRenderTarget.Draw();
	PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

	RenderTargetStatus::Instance()->PrepareToCloseBarrier(gameTexHandle);
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
}