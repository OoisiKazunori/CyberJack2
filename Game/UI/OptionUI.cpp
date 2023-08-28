#include "OptionUI.h"
#include "../KazLibrary/Loader/TextureResourceMgr.h"
#include "../KazLibrary/Render/DrawFunc.h"
#include "../KazLibrary/Input/ControllerInputManager.h"
#include <Imgui/imgui.h>
#include "../Effect/TimeZone.h"
#include "../Effect/SeaEffect.h"
#include "../KazLibrary/Easing/easing.h"

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
	m_headlines.emplace_back(OptionHeadline("RAYTRACING", KazMath::Vec2<float>(0, 0), 32.0f, RAYTRACING));
	m_headlines.emplace_back(OptionHeadline("TIMEZONE", KazMath::Vec2<float>(0, 0), 32.0f, TIMEZONE));
	m_headlines.emplace_back(OptionHeadline("SEA", KazMath::Vec2<float>(0, 0), 32.0f, SEA));
	m_optionUI.emplace_back(OptionHeadline("OPTION", KazMath::Vec2<float>(0, 0), OPTION_FONTSIZE, 0));

	//オプション詳細を設定。
	m_optionDetails.emplace_back(OptionDetails("DEBUG", { DrawStringData("ON"),DrawStringData("OFF") }, KazMath::Vec2<float>(), RAYTRACING));
	m_optionDetails.emplace_back(OptionDetails("TIME", { DrawStringData("NOON"),DrawStringData("EVENING") }, KazMath::Vec2<float>(), TIMEZONE));
	m_optionDetails.emplace_back(OptionDetails("STATE", { DrawStringData("A"),DrawStringData("B"),DrawStringData("C") }, KazMath::Vec2<float>(), SEA));

	//背景をロード
	m_backGroundTexture = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/UI/white.png");
	m_backGroundColor = KazMath::Color(0,0,0,0);
	m_backGroundRender = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_backGroundEasingTimer = 0;

	//各変数の初期設定
	m_nowSelectHeadline = 0;
	m_prevInputUp = false;
	m_prevInputDown = false;
	m_prevInputRight = false;
	m_prevInputLeft = false;
	m_isDisplayUI = false;
	m_isChangeDisplayUI = false;
	m_isRaytracingDebug = false;

}

void OptionUI::Update()
{

	//入力処理
	Input();

	//選択している値によってデバッグを切り替える。
	switch (m_nowSelectHeadline) {
	case RAYTRACING:
	{

		//選択している詳細のIDを反映。
		m_isRaytracingDebug = m_optionDetails[RAYTRACING].m_selectID;

		break;
	}
	case TIMEZONE:
	{

		//選択している詳細のIDを反映。
		TimeZone::Instance()->m_timeZone = m_optionDetails[TIMEZONE].m_selectID;

		break;
	}
	case SEA:
	{

		//選択している詳細のIDを反映。
		SeaEffect::Instance()->m_seaID = m_optionDetails[SEA].m_selectID;

		break;
	}
	default:
		break;
	}

	//表示状態だったら
	if (m_isDisplayUI) {

		//表示を切り替える状態だったら
		if (m_isChangeDisplayUI) {

			//背景のアルファを更新。
			m_backGroundEasingTimer = std::clamp(m_backGroundEasingTimer + 1.0f, 0.0f, BACK_GROUND_EASING_TIMER);
			float backGroundAlphaEasing = EasingMaker(Out, Cubic, m_backGroundEasingTimer / BACK_GROUND_EASING_TIMER);
			m_backGroundColor.color.a = BACK_GROUND_ALPHA - static_cast<int>(BACK_GROUND_ALPHA * backGroundAlphaEasing);

			//すべてのイージングが終わったら。
			if (BACK_GROUND_EASING_TIMER <= m_backGroundEasingTimer) {

				m_isDisplayUI = false;
				m_isChangeDisplayUI = false;
				m_backGroundEasingTimer = 0;

			}

		}

	}
	else {

		//表示を切り替える状態だったら
		if (m_isChangeDisplayUI) {

			//背景のアルファを更新。
			m_backGroundEasingTimer = std::clamp(m_backGroundEasingTimer + 1.0f, 0.0f, BACK_GROUND_EASING_TIMER);
			float backGroundAlphaEasing = EasingMaker(Out, Cubic, m_backGroundEasingTimer / BACK_GROUND_EASING_TIMER);
			m_backGroundColor.color.a = static_cast<int>(BACK_GROUND_ALPHA * backGroundAlphaEasing);

			//すべてのイージングが終わったら。
			if (BACK_GROUND_EASING_TIMER <= m_backGroundEasingTimer) {

				m_isDisplayUI = true;
				m_isChangeDisplayUI = false;
				m_backGroundEasingTimer = 0;

			}

		}


	}


}

void OptionUI::Draw(DrawingByRasterize& arg_rasterize)
{

	//ImGui::Begin("UI");

	//ImGui::DragFloat("X", &DETAIL_BASEPOS.x);
	//ImGui::DragFloat("Y", &DETAIL_BASEPOS.y);
	//ImGui::DragFloat("DETAIL_FLAG_POS", &DETAIL_FLAG_POS);

	//ImGui::End();


	const int ASCII_A = 65;	//"A"のASCIIコード
	const int ASCII_Z = 90;	//"Z"のASCIIコード

	//OPTIONの文字を描画する。
	{
		const int CHARA_COUNT = static_cast<int>(m_optionUI.front().m_name.m_string.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//この文字のFontの番号を調べる。
			int fontNum = static_cast<int>(m_optionUI.front().m_name.m_string[index]) - ASCII_A;

			//フォント数が既定値を超えていたら飛ばす。
			if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

			m_optionUI.front().m_name.m_color[index] = KazMath::Color(255, 255, 255, m_backGroundColor.color.a);

			m_optionUI.front().m_fontSize = OPTION_FONTSIZE;

			//トランスフォームを用意。
			KazMath::Transform2D transform;
			transform.pos = OPTION_BASEPOS;
			transform.pos.x += (m_optionUI.front().m_fontSize / 2.0f) + (m_optionUI.front().m_fontSize * index);
			transform.scale.x = m_optionUI.front().m_fontSize;
			transform.scale.y = m_optionUI.front().m_fontSize;
			DrawFunc::DrawTextureIn2D(m_optionUI.front().m_name.m_render[index], transform, m_font[fontNum], m_optionUI.front().m_name.m_color[index]);
			arg_rasterize.ObjectRender(m_optionUI.front().m_name.m_render[index]);


		}
	}

	//小見出しを描画
	for (auto& headline : m_headlines) {

		const int CHARA_COUNT = static_cast<int>(headline.m_name.m_string.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//この文字のFontの番号を調べる。
			int fontNum = static_cast<int>(headline.m_name.m_string[index]) - ASCII_A;

			//フォント数が既定値を超えていたら飛ばす。
			if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

			headline.m_name.m_color[index] = KazMath::Color(255,255,255, m_backGroundColor.color.a);
			//選択中じゃなかったら色を薄くする。
			if (headline.m_headlineID != m_nowSelectHeadline) {
				headline.m_name.m_color[index] = KazMath::Color(150,150,150, m_backGroundColor.color.a);
			}

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
			DrawFunc::DrawTextureIn2D(headline.m_name.m_render[index], transform, m_font[fontNum], headline.m_name.m_color[index]);
			arg_rasterize.ObjectRender(headline.m_name.m_render[index]);


		}

	}

	//オプションの詳細を描画
	const int CHARA_COUNT = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_name.m_string.size());
	for (int index = 0; index < CHARA_COUNT; ++index) {

		//この文字のFontの番号を調べる。
		int fontNum = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_name.m_string[index]) - ASCII_A;

		//フォント数が既定値を超えていたら飛ばす。
		if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

		m_optionDetails[m_nowSelectHeadline].m_name.m_color[index] = KazMath::Color(255, 255, 255, m_backGroundColor.color.a);

		//トランスフォームを用意。
		KazMath::Transform2D transform;
		transform.pos = DETAIL_BASEPOS;
		transform.pos.x += (DETAIL_FONTSIZE / 2.0f) + (DETAIL_FONTSIZE * index);
		transform.pos.y += DETAIL_FONTSIZE / 2.0f + (BETWEEN_LINES * m_nowSelectHeadline);
		transform.scale.x = DETAIL_FONTSIZE;
		transform.scale.y = DETAIL_FONTSIZE;
		DrawFunc::DrawTextureIn2D(m_optionDetails[m_nowSelectHeadline].m_name.m_render[index], transform, m_font[fontNum], m_optionDetails[m_nowSelectHeadline].m_name.m_color[index]);
		arg_rasterize.ObjectRender(m_optionDetails[m_nowSelectHeadline].m_name.m_render[index]);

	}
	const int NOW_SELECT_DETAIL_ID = m_optionDetails[m_nowSelectHeadline].m_selectID;
	const int DETAIL_CHARA_COUNT = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_string.size());
	for (int index = 0; index < DETAIL_CHARA_COUNT; ++index) {

		//この文字のFontの番号を調べる。
		int fontNum = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_string[index]) - ASCII_A;

		//フォント数が既定値を超えていたら飛ばす。
		if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

		m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_color[index] = KazMath::Color(255, 255, 255, m_backGroundColor.color.a);

		//トランスフォームを用意。
		KazMath::Transform2D transform;
		transform.pos = DETAIL_BASEPOS;
		transform.pos.x += (DETAIL_FONTSIZE / 2.0f) + (DETAIL_FONTSIZE * index) + DETAIL_FLAG_POS;
		transform.pos.y += DETAIL_FONTSIZE / 2.0f + (BETWEEN_LINES * m_nowSelectHeadline);
		transform.scale.x = DETAIL_FONTSIZE;
		transform.scale.y = DETAIL_FONTSIZE;
		DrawFunc::DrawTextureIn2D(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_render[index], transform, m_font[fontNum], m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_color[index]);
		arg_rasterize.ObjectRender(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_render[index]);

	}

	//背景を描画
	{
		KazMath::Transform2D transform;
		transform.pos = KazMath::Vec2<float>(1280.0f / 2.0f, 720.0f / 2.0f);
		transform.scale = KazMath::Vec2<float>(1280.0f, 720.0f);
		DrawFunc::DrawTextureIn2D(m_backGroundRender, transform, m_backGroundTexture, m_backGroundColor);
		arg_rasterize.ObjectRender(m_backGroundRender);
	}

}

void OptionUI::Input()
{
	//スタートボタンが入力されたら。
	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_START)) {
		m_isChangeDisplayUI = true;
	}

	//UIが表示されていない状態だったら入力を切る。
	if (!m_isDisplayUI) return;

	const int DEADLINE = 30000;

	//下方向に入力されたら
	bool isInputDown = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::DOWN_SIDE, DEADLINE);
	if (isInputDown && !m_prevInputDown) {

		m_nowSelectHeadline = std::clamp(m_nowSelectHeadline + 1, 0, static_cast<int>(m_headlines.size()) - 1);

	}
	//上方向に入力されたら
	bool isInputUp = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::UP_SIDE, DEADLINE);
	if (isInputUp && !m_prevInputUp) {

		m_nowSelectHeadline = std::clamp(m_nowSelectHeadline - 1, 0, static_cast<int>(m_headlines.size()) - 1);

	}

	//右方向に入力されたら
	bool isInputRight = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::RIGHT_SIDE, DEADLINE);
	if (isInputRight && !m_prevInputRight) {

		++m_optionDetails[m_nowSelectHeadline].m_selectID;

	}

	//左方向に入力されたら
	bool isInputLeft = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::LEFT_SIDE, DEADLINE);
	if (isInputLeft && !m_prevInputLeft) {

		--m_optionDetails[m_nowSelectHeadline].m_selectID;

	}
	m_optionDetails[m_nowSelectHeadline].m_selectID = std::clamp(m_optionDetails[m_nowSelectHeadline].m_selectID, 0, static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_selectName.size()) - 1);

	//入力を保存。
	m_prevInputDown = isInputDown;
	m_prevInputUp = isInputUp;
	m_prevInputRight = isInputRight;
	m_prevInputLeft = isInputLeft;
}

OptionUI::OptionHeadline::OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID)
	: m_pos(arg_pos), m_fontSize(arg_fontSize), m_headlineID(arg_headlineID)
{

	m_name.m_string = arg_headline;
	for (auto& index : m_name.m_render) {
		index = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	}
	for (auto& index : m_name.m_color) {
		index = KazMath::Color(0, 255, 0, 255);
	}

}

OptionUI::OptionDetails::OptionDetails(std::string arg_name, std::vector<DrawStringData> arg_selectName, KazMath::Vec2<float> arg_pos, int arg_id)
	: m_pos(arg_pos), m_id(arg_id)
{

	m_name.m_string = arg_name;
	for (auto& index : m_name.m_render) {
		index = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	}
	for (auto& index : m_name.m_color) {
		index = KazMath::Color(0, 255, 0, 255);
	}

	const int SELECT_COUNT = static_cast<int>(arg_selectName.size());
	m_selectName.resize(SELECT_COUNT);
	for (int index = 0; index < SELECT_COUNT; ++index) {
		m_selectName[index].m_string = arg_selectName[index].m_string;
		for (auto& index : m_selectName[index].m_render) {
			index = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
		}
		for (auto& index : m_selectName[index].m_color) {
			index = KazMath::Color(0, 255, 0, 255);
		}
	}

	m_selectID = 0;

}
