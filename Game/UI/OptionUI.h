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

	//描画する文字列、描画に必要なDrawCallDataをまとめた構造体
	struct DrawStringData {
		std::string m_string;
		std::array<DrawFuncData::DrawCallData, 32> m_render;
		std::array<KazMath::Color, 32> m_color;
		DrawStringData() {};
		DrawStringData(std::string arg_name) : m_string(arg_name) {};
	};

	//オプションの小見出し用構造体
	struct OptionHeadline {
		DrawStringData m_name;
		KazMath::Vec2<float> m_pos;
		float m_fontSize;
		int m_headlineID;
		OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID);
	};

	//小見出し配列
	std::vector<OptionHeadline> m_headlines;
	std::vector<OptionHeadline> m_optionUI;	//"OPTION"用

	//オプション詳細用構造体
	struct OptionDetails {
		DrawStringData m_name;
		std::vector<DrawStringData> m_selectName;
		KazMath::Vec2<float> m_pos;
		int m_id;
		int m_selectID;
		OptionDetails(std::string arg_name, std::vector<DrawStringData> arg_selectName, KazMath::Vec2<float> arg_pos, int arg_id);
	};

	//オプション詳細用配列
	std::vector<OptionDetails> m_optionDetails;

	//現在選択中の小見出しID
	int m_nowSelectHeadline;

	//入力保存用
	bool m_prevInputUp;
	bool m_prevInputDown;
	bool m_prevInputRight;
	bool m_prevInputLeft;

	//UIを表示するかどうかのフラグ
	bool m_isDisplayUI;
	bool m_isChangeDisplayUI;

	//レイトレのデバッグのフラグ
	bool m_isRaytracingDebug;

	//フォント配列
	std::array<KazBufferHelper::BufferData, 26> m_font;

	//デバッグの表示中の背景を暗くする用のテクスチャ
	DrawFuncData::DrawCallData m_backGroundRender;
	KazMath::Color m_backGroundColor;
	KazBufferHelper::BufferData m_backGroundTexture;
	const int BACK_GROUND_ALPHA = 220;
	float m_backGroundEasingTimer;
	const float BACK_GROUND_EASING_TIMER = 20;

	//UI配置用変数
	const float BETWEEN_LINES = 55.0f;
	const KazMath::Vec2<float> HEADLINE_BASEPOS = KazMath::Vec2<float>(170, 300);
	const float DEFAULT_FONTSIZE = 25.0f;
	const float SELECT_FONTSIZE = 30.0f;
	const float OPTION_FONTSIZE = 45.7f;
	const KazMath::Vec2<float> OPTION_BASEPOS = KazMath::Vec2<float>(143, 240);
	const KazMath::Vec2<float> DETAIL_BASEPOS = KazMath::Vec2<float>(600, 300);
	const float DETAIL_FONTSIZE = 25.0f;
	const float DETAIL_FLAG_POS = 180.0f;	//詳細の右側のONOFFとかを描画する位置。

	//デバッグの名称
	enum DEBUG_NAME {
		RAYTRACING,
		TIMEZONE,
		SEA
	};


public:

	void Setting();

	void Update();

	void Draw(DrawingByRasterize& arg_rasterize);

private:

	void Input();

};