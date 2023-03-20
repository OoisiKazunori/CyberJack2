#include"WindowForAd.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

WindowForAd::WindowForAd()
{
	spriteRender = std::make_unique<Sprite3DRender>();
	spriteRender->data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StageUIPath + "Flame.png");

	spriteRender->data.pipelineName = PIPELINE_NAME_SPRITE_WINODW;
	spriteRender->data.transform.scale = { 1.0f,1.0f,1.0f };
	spriteRender->data.billBoardFlag = true;

	noiseHandle = spriteRender->CreateConstBuffer(sizeof(ObjectData), typeid(ObjectData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
	noiseMaxTimer = KazMath::ConvertSecondToFlame(2);
	exchangeWindowFlag = false;

	spriteRender->data.addHandle.paramType[0] = GRAPHICS_PRAMTYPE_TEX2;

	DirectX::XMFLOAT3 colorRate = KazMath::Color(29, 19, 72, 255).ConvertColorRateToXMFLOAT3();
	data.fogData = { colorRate.x,colorRate.y,colorRate.z,0.0004f };
}

void WindowForAd::Init(KazMath::Vec3<float> &POS, const std::array<RESOURCE_HANDLE, 10> &HANDLE_ARRAY)
{
	windowTexHandleArray = HANDLE_ARRAY;
	spriteRender->data.addHandle.handle[0] = windowTexHandleArray[KazMath::Rand(9, 0)];
	spriteRender->data.transform.pos = POS;
	float lScale = KazMath::Rand(2.5f, 1.0f);
	spriteRender->data.transform.scale = { lScale,lScale,0.0f };

	timer = 0;
	noiseTimer = 0;
	maxTimer = KazMath::Rand(KazMath::ConvertSecondToFlame(10), KazMath::ConvertSecondToFlame(5));
}

void WindowForAd::Update()
{
	//ˆê’èŠÔ‚½‚Á‚½‚ç‰æ‘œ‚ğ“ü‚ê‘Ö‚¦‚é€”õ‚ğ‚·‚é
	if (maxTimer <= timer)
	{
		exchangeWindowFlag = true;
	}
	++timer;

	data.seed = -1.0f;
	spriteRender->TransData(&data, noiseHandle, typeid(data).name());


	spriteRender->data.transform.pos.z += -5.0f;
	if (spriteRender->data.transform.pos.z <= -10000.0f)
	{
		spriteRender->data.transform.pos.z = 10000.0f;
	}



	if (!exchangeWindowFlag)
	{
		return;
	}
	//‰æ‘œ‚ğ“ü‚ê‘Ö‚¦’†

	data.seed = static_cast<float>(noiseTimer);
	spriteRender->TransData(&data, noiseHandle, typeid(data).name());

	++noiseTimer;

	//‰æ‘œ“ü‚ê‘Ö‚¦Š®—¹
	if (noiseMaxTimer <= noiseTimer)
	{
		noiseTimer = 0;
		timer = 0;
		spriteRender->data.addHandle.handle[0] = windowTexHandleArray[KazMath::Rand(9, 0)];
		exchangeWindowFlag = false;
	}
}

void WindowForAd::Draw()
{
	spriteRender->Draw();
}