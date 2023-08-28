#include "OptionUI.h"
#include "../KazLibrary/Loader/TextureResourceMgr.h"
#include "../KazLibrary/Render/DrawFunc.h"
#include "../KazLibrary/Input/ControllerInputManager.h"
#include <Imgui/imgui.h>

void OptionUI::Setting()
{

	//フォントをロード
	int counter = 97; //97は"a"
	std::string path = "Resource/UI/Font/";
	std::string extension = ".png";
	for (auto& index : m_font) {

		//ロードするファイルの文字列を取得。
		std::string filePath = path;
		filePath.push_back(static_cast<char>(counter));
		filePath += extension;

		//ファイルをロード
		index = TextureResourceMgr::Instance()->LoadGraphBuffer(filePath);

		++counter;
	}

	//小見出しを設定。
	m_headlines.emplace_back(OptionHeadline("RAYTRACING", KazMath::Vec2<float>(0, 0), 32.0f, 0));
	m_headlines.emplace_back(OptionHeadline("TIMEZONE", KazMath::Vec2<float>(0, 0), 32.0f, 1));
	m_headlines.emplace_back(OptionHeadline("SEA", KazMath::Vec2<float>(0, 0), 32.0f, 2));
	m_optionUI.emplace_back(OptionHeadline("OPTION", KazMath::Vec2<float>(0, 0), OPTION_FONTSIZE, 0));

	//各変数の初期設定
	m_nowSelectHeadline = 0;
	m_prevInputUp = false;
	m_prevInputDown = false;
	m_isDisplayUI = false;

}

void OptionUI::Update()
{

	//入力処理
	Input();

}

void OptionUI::Draw(DrawingByRasterize& arg_rasterize)
{

	if (!m_isDisplayUI) return;

	//ImGui::Begin("UI");

	//ImGui::DragFloat("BASE_POS_X", &HEADLINE_BASEPOS.x, 0.5f);
	//ImGui::DragFloat("BASE_POS_Y", &HEADLINE_BASEPOS.y, 0.5f);
	//ImGui::DragFloat("Gyokan", &BETWEEN_LINES, 0.1f);
	//ImGui::DragFloat("DEFAULT_FONTSIZE", &DEFAULT_FONTSIZE, 0.1f);
	//ImGui::DragFloat("SELECT_FONTSIZE", &SELECT_FONTSIZE, 0.1f);
	//ImGui::DragFloat("OPTION_POS_X", &OPTION_BASEPOS.x, 0.5f);
	//ImGui::DragFloat("OPTION_POS_Y", &OPTION_BASEPOS.y, 0.5f);
	//ImGui::DragFloat("OPTION_FONTSIZE", &OPTION_FONTSIZE, 0.1f);

	//ImGui::End();


	const int ASCII_A = 65;	//"A"のASCIIコード

	//OPTIONの文字を描画する。
	{
		const int CHARA_COUNT = static_cast<int>(m_optionUI.front().m_headline.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//この文字のFontの番号を調べる。
			int fontNum = static_cast<int>(m_optionUI.front().m_headline[index]) - ASCII_A;

			m_optionUI.front().m_color[index] = KazMath::Color(10, 10, 10, 255);

			m_optionUI.front().m_fontSize = OPTION_FONTSIZE;

			//トランスフォームを用意。
			KazMath::Transform2D transform;
			transform.pos = OPTION_BASEPOS;
			transform.pos.x += (m_optionUI.front().m_fontSize / 2.0f) + (m_optionUI.front().m_fontSize * index);
			transform.scale.x = m_optionUI.front().m_fontSize;
			transform.scale.y = m_optionUI.front().m_fontSize;
			DrawFunc::DrawTextureIn2D(m_optionUI.front().m_render[index], transform, m_font[fontNum], m_optionUI.front().m_color[index]);
			arg_rasterize.ObjectRender(m_optionUI.front().m_render[index]);


		}
	}

	//小見出しを描画
	for (auto& headline : m_headlines) {

		const int CHARA_COUNT = static_cast<int>(headline.m_headline.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//この文字のFontの番号を調べる。
			int fontNum = static_cast<int>(headline.m_headline[index]) - ASCII_A;

			headline.m_color[index] = KazMath::Color(10, 10, 10, 255);

			//使用するフォントのサイズを決める。
			headline.m_fontSize = DEFAULT_FONTSIZE;
			if (headline.m_headlineID == m_nowSelectHeadline) {
				headline.m_fontSize = SELECT_FONTSIZE;
			}

			//トランスフォームを用意。
			KazMath::Transform2D transform;
			transform.pos = HEADLINE_BASEPOS;
			transform.pos.x += (headline.m_fontSize / 2.0f) + (headline.m_fontSize * index);
			transform.pos.y += headline.m_fontSize / 2.0f + (BETWEEN_LINES * headline.m_headlineID);
			transform.scale.x = headline.m_fontSize;
			transform.scale.y = headline.m_fontSize;
			DrawFunc::DrawTextureIn2D(headline.m_render[index], transform, m_font[fontNum], headline.m_color[index]);
			arg_rasterize.ObjectRender(headline.m_render[index]);


		}

	}

}

void OptionUI::Input()
{
	//スタートボタンが入力されたら。
	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_START)) {
		m_isDisplayUI = !m_isDisplayUI;
	}

	//UIが表示されていない状態だったら入力を切る。
	if (!m_isDisplayUI) return;

	//下方向に入力されたら
	bool isInputDown = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::DOWN_SIDE);
	if (isInputDown && !m_prevInputDown) {

		m_nowSelectHeadline = std::clamp(m_nowSelectHeadline + 1, 0, static_cast<int>(m_headlines.size()) - 1);

	}
	//上方向に入力されたら
	bool isInputUp = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::UP_SIDE);
	if (isInputUp && !m_prevInputUp) {

		m_nowSelectHeadline = std::clamp(m_nowSelectHeadline - 1, 0, static_cast<int>(m_headlines.size()) - 1);

	}

	//入力を保存。
	m_prevInputDown = isInputDown;
	m_prevInputUp = isInputUp;
}

OptionUI::OptionHeadline::OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID)
	: m_headline(arg_headline), m_pos(arg_pos), m_fontSize(arg_fontSize), m_headlineID(arg_headlineID)
{

	for (auto& index : m_render) {
		index = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	}
	for (auto& index : m_color) {
		index = KazMath::Color(0, 255, 0, 255);
	}

}
