#include "DrawFuncHelper.h"

DrawFuncHelper::TextureRender::TextureRender(const std::string& arg_textureFilePass)
{
	m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(arg_textureFilePass);
	Error();
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

DrawFuncHelper::TextureRender::TextureRender(const std::string& arg_textureFilePass, const DrawFuncData::DrawCallData& arg_drawCall)
{
	m_drawCommand = arg_drawCall;
	m_textureBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(arg_textureFilePass);
	Error();
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

DrawFuncHelper::TextureRender::TextureRender(const DrawFuncData::DrawCallData& arg_drawCall)
{
	m_drawCommand = arg_drawCall;
}

DrawFuncHelper::TextureRender::TextureRender()
{
	m_drawCommand = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteShader());
}

void DrawFuncHelper::TextureRender::operator=(const KazBufferHelper::BufferData& rhs)
{
	m_textureBuffer = rhs;
	m_textureSize =
	{
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(m_textureBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};
}

void DrawFuncHelper::TextureRender::Draw2D(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D, const KazMath::Color& arg_addColor)
{
	Error();

	KazMath::Transform2D transform(arg_trasform2D);
	//テクスチャのサイズに割合をかける
	transform.scale *= m_textureSize;
	DrawFunc::DrawTextureIn2D(m_drawCommand, transform, m_textureBuffer, arg_addColor);
	arg_rasterize.ObjectRender(m_drawCommand);
}
void DrawFuncHelper::TextureRender::Draw3D(DrawingByRasterize& arg_rasterize, const KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor)
{
	Error();

	KazMath::Transform3D transform(arg_trasform3D);
	//テクスチャのサイズに割合をかける
	transform.scale *= {m_textureSize.x, -m_textureSize.y, 1.0f};
	DrawFunc::DrawTextureIn3D(m_drawCommand, transform, m_textureBuffer, arg_addColor);
	arg_rasterize.ObjectRender(m_drawCommand);
}

void DrawFuncHelper::TextureRender::Error()
{
	if (!m_textureBuffer.bufferWrapper)
	{
		//テクスチャ読み込み失敗
		assert(0);
	}
};

DrawFuncHelper::ModelRender::ModelRender(const std::string& arg_fileDir, const std::string& arg_filePass)
{
	Load(arg_fileDir, arg_filePass);
}

DrawFuncHelper::ModelRender::ModelRender(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall)
{
	m_modelInfo = arg_modelInfomation;
	LoadAnimation();
	m_drawCommand = arg_drawCall;
}

DrawFuncHelper::ModelRender::ModelRender()
{
}

void DrawFuncHelper::ModelRender::Load(const std::string& arg_fileDir, const std::string& arg_filePass)
{
	m_modelInfo = ModelLoader::Instance()->Load(arg_fileDir, arg_filePass);

	Error();
	if (LoadAnimation())
	{
		m_drawCommand = DrawFuncData::SetDrawGLTFAnimationIndexMaterialData(*m_modelInfo, DrawFuncData::GetAnimationModelShader());
	}
	else
	{
		m_drawCommand = DrawFuncData::SetDrawGLTFAnimationIndexMaterialData(*m_modelInfo, DrawFuncData::GetDefferedModelShader());
	}
}

void DrawFuncHelper::ModelRender::Load(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall)
{
	m_modelInfo = arg_modelInfomation;

	Error();
	LoadAnimation();
	m_drawCommand = arg_drawCall;
}

bool DrawFuncHelper::ModelRender::LoadAnimation()
{
	//アニメーションあり
	if (3 < m_modelInfo->skelton->bones.size())
	{
		m_animator = std::make_shared<ModelAnimator>(m_modelInfo);
		return true;
	}
	else
	{
		return false;
	}
}

void DrawFuncHelper::ModelRender::Draw(DrawingByRasterize& arg_rasterize, KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor, float arg_timeScale)
{
	if (m_animator)
	{
		m_animator->Update(arg_timeScale);
		DrawFunc::DrawModel(m_drawCommand, arg_trasform3D, m_animator->GetBoneMatBuff(), arg_addColor);
	}
	else
	{
		DrawFunc::DrawModel(m_drawCommand, arg_trasform3D, arg_addColor);
	}
	arg_rasterize.ObjectRender(m_drawCommand);
}

void DrawFuncHelper::ModelRender::Error()
{
	if (!m_modelInfo)
	{
		//モデル読み込み失敗
		assert(0);
	}
}
