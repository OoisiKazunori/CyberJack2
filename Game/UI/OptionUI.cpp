#include "OptionUI.h"
#include "../KazLibrary/Loader/TextureResourceMgr.h"
#include "../KazLibrary/Render/DrawFunc.h"

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
	m_headlines.emplace_back(OptionHeadline("RAYTRACING", KazMath::Vec2<float>(10, 10), 32.0f, 0));
	m_headlines.emplace_back(OptionHeadline("TIMEZONE", KazMath::Vec2<float>(10, 50), 32.0f, 0));
	m_headlines.emplace_back(OptionHeadline("SEA", KazMath::Vec2<float>(10, 90), 32.0f, 0));

}

void OptionUI::Update()
{
}

void OptionUI::Draw(DrawingByRasterize& arg_rasterize)
{

	const int ASCII_A = 65;	//"A"��ASCII�R�[�h

	//�����o����`��
	for (auto& headline : m_headlines) {

		const int CHARA_COUNT = static_cast<int>(headline.m_headline.size());
		for (int index = 0; index < CHARA_COUNT; ++index) {

			//���̕�����Font�̔ԍ��𒲂ׂ�B
			int fontNum = static_cast<int>(headline.m_headline[index]) - ASCII_A;

			//�g�����X�t�H�[����p�ӁB
			KazMath::Transform2D transform;
			transform.pos = headline.m_pos;
			transform.pos.x += (headline.m_fontSize / 2.0f) + (headline.m_fontSize * index);
			transform.pos.y += headline.m_fontSize / 2.0f;
			transform.scale.x = headline.m_fontSize;
			transform.scale.y = headline.m_fontSize;
			DrawFunc::DrawTextureIn2D(headline.m_render[index], transform, m_font[fontNum]);
			arg_rasterize.ObjectRender(headline.m_render[index]);


		}

	}

}

OptionUI::OptionHeadline::OptionHeadline(std::string arg_headline, KazMath::Vec2<float> arg_pos, float arg_fontSize, int arg_headlineID)
	: m_headline(arg_headline), m_pos(arg_pos), m_fontSize(arg_fontSize), m_headlineID(arg_headlineID)
{
	//m_render.resize(static_cast<int>(arg_headline.size()));
	for (auto& index : m_render) {
		index = DrawFuncData::SetTexPlaneData(DrawFuncData::GetSpriteShader());
	}

	//for (int index = 0; index < static_cast<int>(arg_headline.size()); ++index) {
	//	m_render.emplace_back();
	//	
	//}
}
