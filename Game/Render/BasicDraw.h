#pragma once
#include"Render/DrawFunc.h"
#include"DrawFunc/DrawFuncHelper.h"
#include"Render/DrawingByRasterize.h"
#include"Loader/ModelLoader.h"
#include"Buffer/GBufferMgr.h"

namespace BasicDraw
{
	DrawFuncData::DrawCallData SetModel(const std::shared_ptr<ModelInfomation>& arg_modelInfomation);
	DrawFuncData::DrawCallData SetTex();

	struct BasicModelRender
	{
		BasicModelRender(const std::string& arg_fileDir, const std::string& arg_fileName);
		BasicModelRender();

		void Load(const std::string& arg_fileDir, const std::string& arg_fileName);
		DrawFuncHelper::ModelRender m_model;
	};
	struct BasicTextureRender
	{
		BasicTextureRender(const std::string& arg_filePass);
		DrawFuncHelper::TextureRender m_tex;
	};

};