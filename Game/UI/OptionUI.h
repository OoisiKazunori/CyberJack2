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

	//�`�悷�镶����A�`��ɕK�v��DrawCallData���܂Ƃ߂��\����
	struct DrawStringData {
		std::string m_string;
		std::array<DrawFuncData::DrawCallData, 32> m_render;
		std::array<KazMath::Color, 32> m_color;
		DrawStringData() {};
		DrawStringData(std::string arg_name) : m_string(arg_name) {};
	};

	//�I�v�V�����̏����o���p�\����
	struct OptionHeadline {
		DrawStringData m_name;
		KazMath::Vec2<float> m_pos;
		float m_fontSize;
		int m_headlineID;
		OptionHeadline() {};
		OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID);
	};

	//�����o���z��
	std::vector<OptionHeadline> m_headlines;
	std::vector<OptionHeadline> m_optionUI;	//"OPTION"�p

	//�I�v�V�����ڍחp�\����
	struct OptionDetails {
		DrawStringData m_name;
		std::vector<DrawStringData> m_selectName;
		KazMath::Vec2<float> m_pos;
		int m_id;
		int m_selectID;
		OptionDetails() {
			m_selectID = 0;
		};
		OptionDetails(std::string arg_name, std::vector<OptionDetails> arg_selectName, KazMath::Vec2<float> arg_pos, int arg_id);
	};

	//�I�v�V�����ڍחp�z��
	std::vector<OptionDetails> m_optionDetails;

	//���ݑI�𒆂̏����o��ID
	int m_nowSelectHeadline;

	//���͕ۑ��p
	bool m_prevInputUp;
	bool m_prevInputDown;

	//UI��\�����邩�ǂ����̃t���O
	bool m_isDisplayUI;

	//�t�H���g�z��
	std::array<KazBufferHelper::BufferData, 26> m_font;

	//UI�z�u�p�ϐ�
	float BETWEEN_LINES = 55.0f;
	KazMath::Vec2<float> HEADLINE_BASEPOS = KazMath::Vec2<float>(170, 300);
	float DEFAULT_FONTSIZE = 25.0f;
	float SELECT_FONTSIZE = 30.0f;
	float OPTION_FONTSIZE = 45.7f;
	KazMath::Vec2<float> OPTION_BASEPOS = KazMath::Vec2<float>(143, 240);


public:

	void Setting();

	void Update();

	void Draw(DrawingByRasterize& arg_rasterize);

private:

	void Input();

};