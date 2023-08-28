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

	//オプションの小見出し用構造体
	struct OptionHeadline {
		std::string m_headline;
		KazMath::Vec2<float> m_pos;
		float m_fontSize;
		int m_headlineID;
		std::array<DrawFuncData::DrawCallData, 10> m_render;
		std::array<KazMath::Color, 10> m_color;
		OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID);
	};

	//小見出し配列
	std::vector<OptionHeadline> m_headlines;

	//現在選択中の小見出しID
	int m_nowSelectHeadline;

	//入力保存用
	bool m_prevInputUp;
	bool m_prevInputDown;

	//UIを表示するかどうかのフラグ
	bool m_isDisplayUI;

	//フォント配列
	std::array<KazBufferHelper::BufferData, 26> m_font;

public:

	void Setting();

	void Update();

	void Draw(DrawingByRasterize& arg_rasterize);

private:

	void Input();

};