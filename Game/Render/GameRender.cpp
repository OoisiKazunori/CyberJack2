#include "GameRender.h"

GameRender::GameRender() :
	m_emissivePass(DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAddShader()))
{
	//m_blurEmissive = std::make_unique<PostEffect::GaussianBlur>(RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::EMISSIVE]));

	//Albedo&Normalとエミッシブ合成用--------------------------
	KazMath::Vec2<UINT>winSize(1280, 720);
	m_compositeHandle = RenderTargetStatus::Instance()->CreateRenderTarget(winSize, { 0.0f,0.0f,0.0f }, DXGI_FORMAT_R8G8B8A8_UNORM);

	m_albedoAndNormalPass = RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::ALBEDO]);
	m_albedoAndNormalPass.m_drawCommand.pipelineData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;

	m_emissivePass = RenderTargetStatus::Instance()->GetBuffer(GBufferMgr::Instance()->GetRenderTarget()[GBufferMgr::EMISSIVE]);
	m_albedoAndNormalPass.m_drawCommand.renderTargetHandle = m_compositeHandle;
	m_emissivePass.m_drawCommand.renderTargetHandle = m_compositeHandle;

	//最終合成--------------------------
	m_finalPass = RenderTargetStatus::Instance()->GetBuffer(m_compositeHandle);

	m_transform.pos = { 1280.0f / 2.0f,720.0f / 2.0f };
}

void GameRender::DrawPass(DrawingByRasterize& arg_rasterize)
{
	//m_blurEmissive->ApplyBlur();

	m_albedoAndNormalPass.Draw2D(arg_rasterize, m_transform);
	m_emissivePass.Draw2D(arg_rasterize, m_transform);

	m_finalPass.Draw2D(arg_rasterize, m_transform);
}
