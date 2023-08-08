#include"../Scene/SceneChange.h"
#include"../Scene/SceneChange.h"
#include"../Helper/ResourceFilePass.h"
#include"../Loader/TextureResourceMgr.h"
#include "../Easing/easing.h"
#include "../Imgui/MyImgui.h"

using namespace ChangeScene;

SceneChange::SceneChange() :allHidenFlag(false), startFlag(false)
{
	texBuffer = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::SceneChangePath + "SceneChange.png");
	transform.scale = {
		static_cast<float>(texBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Width),
		static_cast<float>(texBuffer.bufferWrapper->GetBuffer().Get()->GetDesc().Height)
	};

	DrawFuncData::PipelineGenerateData lData;
	lData.desc = DrawFuncPipelineData::SetTex();
	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "VSmain", "vs_6_4", SHADER_TYPE_VERTEX);
	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Sprite.hlsl", "PSmain", "ps_6_4", SHADER_TYPE_PIXEL);
	lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::NONE;
	sceneTex = DrawFuncData::SetTexPlaneData(lData);
}

void SceneChange::Init()
{
	startFlag = false;
}

void SceneChange::Finalize()
{
	startFlag = false;
}

void SceneChange::Update()
{
	if (startFlag)
	{
		//“ü‚è
		if (startOutInT[0] < 1.0f)
		{
			Rate(&startOutInT[0], 0.03f, 1.0f);
			transform.pos.x = -WIN_X + EasingMaker(Out, Cubic, startOutInT[0]) * WIN_X;
			tmp = transform.pos.x;
		}
		//I‚í‚è
		else
		{
			//‰B‚ê‚½ƒtƒ‰ƒO
			if (!initFlag)
			{
				allHidenFlag = true;
				initFlag = true;
			}

			Rate(&startOutInT[1], 0.03f, 1.0f);
			transform.pos.x = tmp + EasingMaker(In, Cubic, startOutInT[1]) * static_cast<float>(WIN_X);
		}

		if (1.0 <= startOutInT[1])
		{
			startFlag = false;
		}
	}
	else
	{
		initFlag = false;
		startOutInT[0] = 0;
		startOutInT[1] = 0;
		transform.pos.x = -WIN_X;
	}
}

void SceneChange::Draw(DrawingByRasterize& arg_rasterize)
{
	DrawFunc::DrawTextureIn2D(sceneTex, transform, texBuffer);
	arg_rasterize.ObjectRender(sceneTex);
}

void SceneChange::Start()
{
	startFlag = true;
}

bool SceneChange::AllHiden()
{
	if (allHidenFlag)
	{
		allHidenFlag = false;
		return true;
	}
	return false;
}
