#include "LineCpuEffect.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Easing/easing.h"

int LineCpuEffect::LINE_ID = 0;

LineCpuEffect::LineCpuEffect()
{
	for (int i = 0; i < lineRender.size(); ++i)
	{
		lineRender[i].data.pipelineName = PIPELINE_NAME_LINE_FLASHEFFECT;
		constBufferHandle[i] = lineRender[i].CreateConstBuffer(sizeof(DirectX::XMFLOAT4), typeid(DirectX::XMFLOAT4).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);

		lineEffectData[i].x = 0.0f;
		lineEffectData[i].y = 1.0f;
		lineEffectData[i].z = 0.0f;
		lineEffectData[i].w = 120.0f;
		flashTimer[i] = 0;
	}
	maxTimer = 60;


	name = "Line" + std::to_string(LINE_ID);
	id = LINE_ID;
	++LINE_ID;
}

void LineCpuEffect::Init(LineEffectVec VEC, KazMath::Vec3<float> &POS)
{
	lineRender[0].data.startPos = POS;
	lineRender[0].data.endPos = POS;
	KazMath::Vec3<float>lEndPos = POS;

	switch (VEC)
	{
	case LINE_UPVEC:
		lEndPos += KazMath::Vec3<float>(0.0f, KazMath::Rand<float>(10.0f, 3.0f), 0.0f);
		break;
	case LINE_DOWNVEC:
		lEndPos += KazMath::Vec3<float>(0.0f, -KazMath::Rand<float>(10.0f, 3.0f), 0.0f);
		break;
	case LINE_LEFTVEC:
		lEndPos += KazMath::Vec3<float>(-KazMath::Rand<float>(10.0f, 3.0f), 0.0f, 0.0f);
		break;
	case LINE_RIGHTVEC:
		lEndPos += KazMath::Vec3<float>(KazMath::Rand<float>(10.0f, 3.0f), 0.0f, 0.0f);
		break;
	default:
		break;
	}
	endPos = lEndPos;
	flashFlag = false;


	for (int i = 0; i < flashTimer.size(); ++i)
	{
		flashTimer[i] = 0;
	}

	for (int i = 0; i < lineRender.size(); ++i)
	{
		lineEffectData[i].w = static_cast<float>(flashTimer[i]) / static_cast<float>(maxTimer);
		lineRender[i].TransData(&lineEffectData[i], constBufferHandle[i], typeid(DirectX::XMFLOAT4).name());
	}

	circleRender.data.transform.pos = endPos;
	circleRender.data.transform.pos.z -= 0.1f;
	circleRender.data.radius = 0.5f;
	circleRender.data.change3DFlag = true;
	circleRender.data.colorData.color.a = 0;

	appearTimer = 0;
	maxAppearTimer = 30;
	startToAppearFlag = false;

	circleMaxAppearTimer = 60;
	circleAppearTimer = 0;
}

void LineCpuEffect::Update()
{
	if (flashFlag)
	{
		//フラッシュ用のデータ
		for (int i = 0; i < lineRender.size(); ++i)
		{
			lineEffectData[i].w = KazMath::ConvertTimerToRate(flashTimer[i], maxTimer);
			lineRender[i].TransData(&lineEffectData[i], constBufferHandle[i], typeid(DirectX::XMFLOAT4).name());
			++flashTimer[i];
		}
	}
	else
	{
		for (int i = 0; i < lineRender.size(); ++i)
		{
			lineEffectData[i].w = -1.0f;
			lineRender[i].TransData(&lineEffectData[i], constBufferHandle[i], typeid(DirectX::XMFLOAT4).name());
		}
	}

	//登場演出-------------------------
	if (startToAppearFlag)
	{
		if (maxAppearTimer <= appearTimer)
		{
			appearTimer = maxAppearTimer;
			if (circleMaxAppearTimer <= circleAppearTimer)
			{
				circleAppearTimer = circleMaxAppearTimer;
			}
			else
			{
				++circleAppearTimer;
			}
			circleRender.data.colorData.color.a = static_cast<int>(EasingMaker(Out, Quint, KazMath::ConvertTimerToRate(circleAppearTimer, circleMaxAppearTimer)) * 255.0f);
		}
		else
		{
			++appearTimer;
		}
	}

	endPos.z = basePos.z;
	lineRender[0].data.startPos.z = basePos.z;
	circleRender.data.transform.pos.z = basePos.z - 0.1f;

	KazMath::Vec3<float> lDistance = endPos - lineRender[0].data.startPos;
	lineRender[0].data.endPos = lineRender[0].data.startPos + lDistance * EasingMaker(Out, Quint, KazMath::ConvertTimerToRate(appearTimer, maxAppearTimer));
	//登場演出-------------------------

}

void LineCpuEffect::Draw()
{

	/*ImGui::Begin(name.c_str());
	for (int i = 0; i < lineRender.size(); ++i)
	{
		if (ImGui::TreeNode(std::to_string(i).c_str()))
		{
			ImGui::InputFloat("StartX", &lineRender[i].data.startPos.x);
			ImGui::InputFloat("StartY", &lineRender[i].data.startPos.y);
			ImGui::InputFloat("StartZ", &lineRender[i].data.startPos.z);
			ImGui::InputFloat("EndX", &lineRender[i].data.endPos.x);
			ImGui::InputFloat("EndY", &lineRender[i].data.endPos.y);
			ImGui::InputFloat("EndZ", &lineRender[i].data.endPos.z);
			ImGui::TreePop();
		}
	}
	ImGui::End();*/



	for (int i = 0; i < lineRender.size(); ++i)
	{
		lineRender[i].Draw();
	}
	circleRender.Draw();
}

void LineCpuEffect::FlashLight()
{
	flashFlag = true;
}

void LineCpuEffect::InitFlash()
{
	flashFlag = false;
	for (int i = 0; i < lineRender.size(); ++i)
	{
		flashTimer[i] = 0;
	}
}

void LineCpuEffect::Appear()
{
	startToAppearFlag = true;
}
