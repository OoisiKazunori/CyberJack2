#include "SearchLight.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

SearchLight::SearchLight()
{
	lightTextureR.data.pipelineName = PIPELINE_NAME_SPRITE_COLOR_FOG;
	lightTextureR.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "Light.png");
}

void SearchLight::Init(const KazMath::Vec3<float> &POS, const KazMath::Color &COLOR)
{
	lightTextureR.data.transform.pos = POS;
	lightTextureR.data.transform.scale = { 10.0f,5.0f,1.0f };
	lightTextureR.data.colorData = COLOR;
	lightTextureR.data.billBoardFlag = true;
	sinTimer = 0.0f;
	sinTimeVel = KazMath::Rand(1.0f, 0.5f);
	length = KazMath::Rand(50.0f, 25.0f);
}

void SearchLight::Update()
{
	sinTimer += sinTimeVel;
	lightTextureR.data.transform.rotation.z = sinf(KazMath::PI_2F / 120.0f * sinTimer) * length;

	lightTextureR.data.transform.pos.z += -5.0f;
	if (lightTextureR.data.transform.pos.z <= -5000.0f)
	{
		lightTextureR.data.transform.pos.z = 5000.0f;
		lightTextureR.data.colorData.color.a = 0;
	}

	if (lightTextureR.data.colorData.color.a < 255)
	{
		lightTextureR.data.colorData.color.a += 5;
	}
}

void SearchLight::Draw()
{
	lightTextureR.Draw();
}
