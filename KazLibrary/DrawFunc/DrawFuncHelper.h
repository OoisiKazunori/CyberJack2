#pragma once
#include"Render/DrawFunc.h"
#include"Render/DrawingByRasterize.h"
#include"Loader/ModelLoader.h"

namespace DrawFuncHelper
{
	//スプライト描画のラッパー
	struct TextureRender
	{
		DrawFuncData::DrawCallData m_drawCommand;
		KazBufferHelper::BufferData m_textureBuffer;
		KazMath::Vec2<float> m_textureSize;

		/// <summary>
		/// テクスチャ読み込み
		/// </summary>
		/// <param name="arg_textureFilePass">テクスチャのファイルパス</param>
		/// <param name="arg_alphaFlag">透過有り無し</param>
		TextureRender(const std::string& arg_textureFilePass);
		/// <summary>
		/// テクスチャ読み込みと描画情報の設定
		/// </summary>
		/// <param name="arg_textureFilePass">テクスチャのファイルパス</param>
		/// <param name="arg_drawCall">描画情報の生成</param>
		TextureRender(const std::string& arg_textureFilePass, const DrawFuncData::DrawCallData& arg_drawCall);
		TextureRender(const DrawFuncData::DrawCallData& arg_drawCall);
		TextureRender();


		void operator=(const KazBufferHelper::BufferData& rhs);

		/// <summary>
		/// スプライト2D描画
		/// </summary>
		void Draw2D(DrawingByRasterize& arg_rasterize, const KazMath::Transform2D& arg_trasform2D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));
		/// <summary>
		/// スプライト3D描画
		/// </summary>
		void Draw3D(DrawingByRasterize& arg_rasterize, const KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255));

		void Error();
	};


	struct ModelRender
	{
		DrawFuncData::DrawCallData m_drawCommand;
		std::shared_ptr<ModelInfomation> m_modelInfo;
		std::shared_ptr<ModelAnimator> m_animator;

		ModelRender(const std::string& arg_fileDir, const std::string& arg_filePass);
		ModelRender(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall);
		ModelRender();

		void Load(const std::string& arg_fileDir, const std::string& arg_filePass);
		void Load(const std::shared_ptr<ModelInfomation>& arg_modelInfomation, const DrawFuncData::DrawCallData& arg_drawCall);

		bool LoadAnimation();

		void Draw(DrawingByRasterize& arg_rasterize, KazMath::Transform3D& arg_trasform3D, const KazMath::Color& arg_addColor = KazMath::Color(255, 255, 255, 255), float arg_timeScale = 1.0f);
		void Error();

	};

}