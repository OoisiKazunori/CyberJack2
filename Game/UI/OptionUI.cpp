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

	//�t�H���g�����[�h
	int counter = 97; //97��"a"
	std::string path = "Resource/UI/Font/";
	std::string extension = ".png";
	for (auto& index : m_font) {

		//���[�h����t�@�C���̕�������擾�B
		std::string filePath = path;
		filePath.push_back(static_cast<char>(counter));
		filePath += extension;

		//�t�@�C�������[�h
		index = TextureResourceMgr::Instance()->LoadGraphBuffer(filePath);

		++counter;
	}

	//�����o����ݒ�B
	m_headlines.emplace_back(OptionHeadline("RAYTRACING", KazMath::Vec2<float>(0, 0), 32.0f, RAYTRACING));
	m_headlines.emplace_back(OptionHeadline("TIMEZONE", KazMath::Vec2<float>(0, 0), 32.0f, TIMEZONE));
	m_headlines.emplace_back(OptionHeadline("SEA", KazMath::Vec2<float>(0, 0), 32.0f, SEA));
	m_optionUI.emplace_back(OptionHeadline("OPTION", KazMath::Vec2<float>(0, 0), OPTION_FONTSIZE, 0));

	//�I�v�V�����ڍׂ�ݒ�B
	m_optionDetails.emplace_back(OptionDetails("DEBUG", { DrawStringData("ON"),DrawStringData("OFF") }, KazMath::Vec2<float>(), RAYTRACING));
	m_optionDetails.emplace_back(OptionDetails("TIME", { DrawStringData("NOON"),DrawStringData("EVENING") }, KazMath::Vec2<float>(), TIMEZONE));
	m_optionDetails.emplace_back(OptionDetails("STATE", { DrawStringData("A"),DrawStringData("B"),DrawStringData("C") }, KazMath::Vec2<float>(), SEA));

	//�w�i�����[�h
	m_backGroundTexture = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/UI/white.png");
	m_backGroundColor = KazMath::Color(0,0,0,0);
	m_backGroundRender = DrawFuncData::SetSpriteAlphaData(DrawFuncData::GetSpriteAlphaShader());
	m_backGroundEasingTimer = 0;

	//�e�ϐ��̏����ݒ�
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

	//���͏���
	Input();

	//�I�����Ă���l�ɂ���ăf�o�b�O��؂�ւ���B
	switch (m_nowSelectHeadline) {
	case RAYTRACING:
	{

		//�I�����Ă���ڍׂ�ID�𔽉f�B
		m_isRaytracingDebug = m_optionDetails[RAYTRACING].m_selectID;

		break;
	}
	case TIMEZONE:
	{

		//�I�����Ă���ڍׂ�ID�𔽉f�B
		TimeZone::Instance()->m_timeZone = m_optionDetails[TIMEZONE].m_selectID;

		break;
	}
	case SEA:
	{

		//�I�����Ă���ڍׂ�ID�𔽉f�B
		SeaEffect::Instance()->m_seaID = m_optionDetails[SEA].m_selectID;

		break;
	}
	default:
		break;
	}

	//�\����Ԃ�������
	if (m_isDisplayUI) {

		//�\����؂�ւ����Ԃ�������
		if (m_isChangeDisplayUI) {

			//�w�i�̃A���t�@���X�V�B
			m_backGroundEasingTimer = std::clamp(m_backGroundEasingTimer + 1.0f, 0.0f, BACK_GROUND_EASING_TIMER);
			float backGroundAlphaEasing = EasingMaker(Out, Cubic, m_backGroundEasingTimer / BACK_GROUND_EASING_TIMER);
			m_backGroundColor.color.a = BACK_GROUND_ALPHA - static_cast<int>(BACK_GROUND_ALPHA * backGroundAlphaEasing);

			//���ׂẴC�[�W���O���I�������B
			if (BACK_GROUND_EASING_TIMER <= m_backGroundEasingTimer) {

				m_isDisplayUI = false;
				m_isChangeDisplayUI = false;
				m_backGroundEasingTimer = 0;

			}

		}

	}
	else {

		//�\����؂�ւ����Ԃ�������
		if (m_isChangeDisplayUI) {

			//�w�i�̃A���t�@���X�V�B
			m_backGroundEasingTimer = std::clamp(m_backGroundEasingTimer + 1.0f, 0.0f, BACK_GROUND_EASING_TIMER);
			float backGroundAlphaEasing = EasingMaker(Out, Cubic, m_backGroundEasingTimer / BACK_GROUND_EASING_TIMER);
			m_backGroundColor.color.a = static_cast<int>(BACK_GROUND_ALPHA * backGroundAlphaEasing);

			//���ׂẴC�[�W���O���I�������B
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


	const int ASCII_A = 65;	//"A"��ASCII�R�[�h
	const int ASCII_Z = 90;	//"Z"��ASCII�R�[�h

	//OPTION�̕�����`�悷��B
	{
		const int CHARA_COUNT = static_cast<int>(m_optionUI.front().m_name.m_string.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//���̕�����Font�̔ԍ��𒲂ׂ�B
			int fontNum = static_cast<int>(m_optionUI.front().m_name.m_string[index]) - ASCII_A;

			//�t�H���g��������l�𒴂��Ă������΂��B
			if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

			m_optionUI.front().m_name.m_color[index] = KazMath::Color(255, 255, 255, m_backGroundColor.color.a);

			m_optionUI.front().m_fontSize = OPTION_FONTSIZE;

			//�g�����X�t�H�[����p�ӁB
			KazMath::Transform2D transform;
			transform.pos = OPTION_BASEPOS;
			transform.pos.x += (m_optionUI.front().m_fontSize / 2.0f) + (m_optionUI.front().m_fontSize * index);
			transform.scale.x = m_optionUI.front().m_fontSize;
			transform.scale.y = m_optionUI.front().m_fontSize;
			DrawFunc::DrawTextureIn2D(m_optionUI.front().m_name.m_render[index], transform, m_font[fontNum], m_optionUI.front().m_name.m_color[index]);
			arg_rasterize.ObjectRender(m_optionUI.front().m_name.m_render[index]);


		}
	}

	//�����o����`��
	for (auto& headline : m_headlines) {

		const int CHARA_COUNT = static_cast<int>(headline.m_name.m_string.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//���̕�����Font�̔ԍ��𒲂ׂ�B
			int fontNum = static_cast<int>(headline.m_name.m_string[index]) - ASCII_A;

			//�t�H���g��������l�𒴂��Ă������΂��B
			if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

			headline.m_name.m_color[index] = KazMath::Color(255,255,255, m_backGroundColor.color.a);
			//�I�𒆂���Ȃ�������F�𔖂�����B
			if (headline.m_headlineID != m_nowSelectHeadline) {
				headline.m_name.m_color[index] = KazMath::Color(150,150,150, m_backGroundColor.color.a);
			}

			//�g�p����t�H���g�̃T�C�Y�����߂�B
			headline.m_fontSize = DEFAULT_FONTSIZE;
			if (headline.m_headlineID == m_nowSelectHeadline) {
				headline.m_fontSize = SELECT_FONTSIZE;
			}

			//�g�����X�t�H�[����p�ӁB
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

	//�I�v�V�����̏ڍׂ�`��
	const int CHARA_COUNT = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_name.m_string.size());
	for (int index = 0; index < CHARA_COUNT; ++index) {

		//���̕�����Font�̔ԍ��𒲂ׂ�B
		int fontNum = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_name.m_string[index]) - ASCII_A;

		//�t�H���g��������l�𒴂��Ă������΂��B
		if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

		m_optionDetails[m_nowSelectHeadline].m_name.m_color[index] = KazMath::Color(255, 255, 255, m_backGroundColor.color.a);

		//�g�����X�t�H�[����p�ӁB
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

		//���̕�����Font�̔ԍ��𒲂ׂ�B
		int fontNum = static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_string[index]) - ASCII_A;

		//�t�H���g��������l�𒴂��Ă������΂��B
		if (fontNum < 0 || static_cast<int>(m_font.size()) <= fontNum) continue;

		m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_color[index] = KazMath::Color(255, 255, 255, m_backGroundColor.color.a);

		//�g�����X�t�H�[����p�ӁB
		KazMath::Transform2D transform;
		transform.pos = DETAIL_BASEPOS;
		transform.pos.x += (DETAIL_FONTSIZE / 2.0f) + (DETAIL_FONTSIZE * index) + DETAIL_FLAG_POS;
		transform.pos.y += DETAIL_FONTSIZE / 2.0f + (BETWEEN_LINES * m_nowSelectHeadline);
		transform.scale.x = DETAIL_FONTSIZE;
		transform.scale.y = DETAIL_FONTSIZE;
		DrawFunc::DrawTextureIn2D(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_render[index], transform, m_font[fontNum], m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_color[index]);
		arg_rasterize.ObjectRender(m_optionDetails[m_nowSelectHeadline].m_selectName[NOW_SELECT_DETAIL_ID].m_render[index]);

	}

	//�w�i��`��
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
	//�X�^�[�g�{�^�������͂��ꂽ��B
	if (ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_START)) {
		m_isChangeDisplayUI = true;
	}

	//UI���\������Ă��Ȃ���Ԃ���������͂�؂�B
	if (!m_isDisplayUI) return;

	const int DEADLINE = 30000;

	//�������ɓ��͂��ꂽ��
	bool isInputDown = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::DOWN_SIDE, DEADLINE);
	if (isInputDown && !m_prevInputDown) {

		m_nowSelectHeadline = std::clamp(m_nowSelectHeadline + 1, 0, static_cast<int>(m_headlines.size()) - 1);

	}
	//������ɓ��͂��ꂽ��
	bool isInputUp = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::UP_SIDE, DEADLINE);
	if (isInputUp && !m_prevInputUp) {

		m_nowSelectHeadline = std::clamp(m_nowSelectHeadline - 1, 0, static_cast<int>(m_headlines.size()) - 1);

	}

	//�E�����ɓ��͂��ꂽ��
	bool isInputRight = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::RIGHT_SIDE, DEADLINE);
	if (isInputRight && !m_prevInputRight) {

		++m_optionDetails[m_nowSelectHeadline].m_selectID;

	}

	//�������ɓ��͂��ꂽ��
	bool isInputLeft = ControllerInputManager::Instance()->InputStickState(ControllerStickSide::LEFT_STICK, ControllerSide::LEFT_SIDE, DEADLINE);
	if (isInputLeft && !m_prevInputLeft) {

		--m_optionDetails[m_nowSelectHeadline].m_selectID;

	}
	m_optionDetails[m_nowSelectHeadline].m_selectID = std::clamp(m_optionDetails[m_nowSelectHeadline].m_selectID, 0, static_cast<int>(m_optionDetails[m_nowSelectHeadline].m_selectName.size()) - 1);

	//���͂�ۑ��B
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
