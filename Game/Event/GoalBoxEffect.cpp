#include "GoalBoxEffect.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"

GoalBoxEffect::GoalBoxEffect()
{
	RESOURCE_HANDLE handle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::GoalPath + "GoalEffect.png");
	for (int i = 0; i < lightRender.size(); ++i)
	{
		lightRender[i] = std::make_unique<Sprite3DRender>(KazMath::Vec2<float>(0.0f, 0.0f));
		lightRender[i]->data.handleData = handle;
		lightRender[i]->data.pipelineName = PIPELINE_NAME_SPRITE_GOAL_EFFECT_MULTITEX;
		uvHandle[i] = lightRender[i]->CreateConstBuffer(sizeof(GoalLightData), typeid(GoalLightData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	}


	lightRender[0]->data.transform.pos = { 0.0f,-0.5f,0.0f };
	lightRender[1]->data.transform.pos = { -0.5f,0.0f,0.0f };
	lightRender[2]->data.transform.pos = { 0.0f,0.5f,0.0f };
	lightRender[3]->data.transform.pos = { 0.5f,0.0f,0.0f };
	lightRender[4]->data.transform.pos = { 0.5f,0.0f,0.0f };
	lightRender[5]->data.transform.pos = { 0.0f,-0.2f,0.0f };
	lightRender[6]->data.transform.pos = { -0.5f,0.0f,0.0f };
	lightRender[7]->data.transform.pos = { 0.0f,0.0f,0.0f };

	lightRender[0]->data.transform.rotation = { 0.0f,-40.0f,90.0f };
	lightRender[1]->data.transform.rotation = { -70.0f,0.0f,0.0f };
	lightRender[2]->data.transform.rotation = { 0.0f,40.0f,270.0f };
	lightRender[3]->data.transform.rotation = { -50.0f,0.0f,180.0f };
	lightRender[4]->data.transform.rotation = { 0.0f,180.0f,10.0f };
	lightRender[5]->data.transform.rotation = { -10.0f,0.0f,130.0f };
	lightRender[6]->data.transform.rotation = { 10.0f,40.0f,50.0f };
	lightRender[7]->data.transform.rotation = { 0.0f,40.0f,230.0f };

	disappearFlag = false;
	motherPtr = nullptr;
	drawHandle = 0;
}

void GoalBoxEffect::Init(DirectX::XMMATRIX *ADRESS)
{
	motherPtr = ADRESS;
	uvData.light = { 0.0f,0.0f,0.0f,0.0f };
	drawHandle = 0;

	for (int i = 0; i < lightRender.size(); ++i)
	{
		lightRender[i]->data.colorData.color.a = 0;
	}
	disappearFlag = false;
}

void GoalBoxEffect::Update()
{
	//êeéqä÷åW
	for (int i = 0; i < lightRender.size(); ++i)
	{
		lightRender[i]->data.motherMat = *motherPtr;
	}


	uvData.light.y += 0.01f;
	for (int i = 0; i < lightRender.size(); ++i)
	{
		lightRender[i]->TransData(&uvData, uvHandle[i], typeid(GoalLightData).name());
	}


	//ìoèÍÇ≥ÇπÇÈ
	if (!disappearFlag)
	{
		for (int i = 0; i < drawHandle; ++i)
		{
			if (lightRender[i]->data.colorData.color.a < 255)
			{
				lightRender[i]->data.colorData.color.a += 5;
			}
			if (255 <= lightRender[i]->data.colorData.color.a)
			{
				lightRender[i]->data.colorData.color.a = 255;
			}
		}
	}
	//è¡Ç¶ÇÈ
	else
	{
		for (int i = 0; i < drawHandle; ++i)
		{
			if (0 <= lightRender[i]->data.colorData.color.a)
			{
				lightRender[i]->data.colorData.color.a -= 5;
			}
			if (lightRender[i]->data.colorData.color.a <= 0)
			{
				lightRender[i]->data.colorData.color.a = 0;
			}
		}
	}

	//ImGui::Begin("Effect");
	//for (int i = 0; i < lightRender.size(); ++i)
	//{
	//	{
	//		std::string name = "X" + std::to_string(i);
	//		ImGui::InputFloat(name.c_str(), &lightRender[i]->data.transform.pos.x);
	//	}
	//	{
	//		std::string name = "Y" + std::to_string(i);
	//		ImGui::InputFloat(name.c_str(), &lightRender[i]->data.transform.pos.y);
	//	}
	//	{
	//		std::string name = "Z" + std::to_string(i);
	//		ImGui::InputFloat(name.c_str(), &lightRender[i]->data.transform.pos.z);
	//	}
	//}
	//ImGui::End();
}

void GoalBoxEffect::Draw()
{
	for (int i = 0; i < lightRender.size(); ++i)
	{
		if (0 < lightRender[i]->data.colorData.color.a)
		{
			lightRender[i]->Draw();
		}
	}
}

void GoalBoxEffect::Appear()
{
	++drawHandle;

	if (8 <= drawHandle)
	{
		drawHandle = 8;
	}
}

void GoalBoxEffect::Disappear()
{
	disappearFlag = true;
}
