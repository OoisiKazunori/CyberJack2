#pragma once
#include "../KazLibrary/Helper/ISinglton.h"
#include "../KazLibrary/Helper/KazBufferHelper.h"
#include "../KazLibrary/Math/KazMath.h"
#include "../KazLibrary/Render/DrawFunc.h"
#include "Render/DrawingByRasterize.h"
#include <string>
#include <memory>
#include <vector>
#include <Buffer/DrawFuncData.h>

class OptionUI :public ISingleton<OptionUI>
{

public:

	//�I�v�V�����̏����o���p�\����
	struct OptionHeadline {
		std::string m_headline;
		KazMath::Vec2<float> m_pos;
		float m_fontSize;
		int m_headlineID;
		std::array<DrawFuncData::DrawCallData, 10> m_render;
		OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID);
	};

	//�����o���z��
	std::vector<OptionHeadline> m_headlines;

	//�t�H���g�z��
	std::array<KazBufferHelper::BufferData, 26> m_font;

public:

	void Setting();

	void Update();

	void Draw(DrawingByRasterize& arg_rasterize);

};