#include"WindowFlame.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Helper/KazHelper.h"

WindowFlame::WindowFlame()
{
	flameTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StageUIPath + "Flame.png");
	flame2DTex.data.handleData = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StageUIPath + "Flame.png");
	flameTex.data.transform.scale = { 0.0f,0.0f,0.0f };
}

void WindowFlame::Init(const KazMath::Transform3D &TRANSFORM, const  KazMath::Vec2<float> &SCALE, bool APPEAR_WINDOW, WindowType TYPE)
{
	KazMath::Transform3D init;
	init.pos = { 0.0f,0.0f,0.0f };
	init.scale = { 0.0f,0.0f,1.0f };
	init.rotation = { 0.0f,0.0f,0.0f };
	flameTex.data.transform = init;

	basePos = TRANSFORM.pos;
	lerpMaxScale = SCALE;
	flameTex.data.transform.rotation = TRANSFORM.rotation;

	flame2DTex.data.transform.pos = {};
	flame2DTex.data.transform.scale = {};


	if (APPEAR_WINDOW)
	{
		flameTex.data.transform.scale = { SCALE.x,SCALE.y,1.0f };
	}
	larpScale = { 0.0f,0.1f,1.0f };
	startFlag = false;
	endFlag = false;
	drawFlag = false;
	readyToWriteFlag = false;
	windowType = TYPE;
}

void WindowFlame::Update()
{
	switch (windowType)
	{
	case WINDOW_2D:
		Update2D();
		break;
	case WINDOW_3D:
		Update3D();
		break;
	default:
		break;
	}
}

void WindowFlame::Draw()
{
	if (drawFlag)
	{
		switch (windowType)
		{
		case WINDOW_2D:
			flame2DTex.Draw();
			break;
		case WINDOW_3D:
			flameTex.Draw();
			break;
		default:
			break;
		}
	}
}

void WindowFlame::Start()
{
	startFlag = true;
	drawFlag = true;
}

void WindowFlame::End()
{
	endFlag = true;
}

bool WindowFlame::IsEnd()
{
	return !drawFlag && endFlag;
}

bool WindowFlame::ReadyToWrite()
{
	return readyToWriteFlag;
}

void WindowFlame::Update2D()
{
	KazMath::Vec2<float>lBasePos = { basePos.x,basePos.y };
	flame2DTex.data.transform.pos = lBasePos;

	if (startFlag)
	{
		larpScale.x = lerpMaxScale.x;

		float mul = 0.3f;
		//ÉEÉBÉìÉhÉEÇâ°Ç…çLÇ∞ÇÈ
		KazMath::Larp(larpScale.x, &flame2DTex.data.transform.scale.x, mul);

		if (lerpMaxScale.x - 0.1f <= flame2DTex.data.transform.scale.x)
		{
			flame2DTex.data.transform.scale.x = lerpMaxScale.x;
		}
		//ÉEÉBÉìÉhÉEÇècÇ…çLÇ∞ÇÈ
		if (larpScale.x <= flame2DTex.data.transform.scale.x)
		{
			larpScale.y = lerpMaxScale.y;
		}
		KazMath::Larp(larpScale.y, &flame2DTex.data.transform.scale.y, mul);

		//ècÇ…çLÇ™Ç¡ÇΩ
		if (lerpMaxScale.y - 0.09f <= flame2DTex.data.transform.scale.y)
		{
			startFlag = false;
			readyToWriteFlag = true;
		}
	}


	//ÇµÇŒÇÁÇ≠ï`âÊÇµÇΩÇÁìoèÍââèoÇ∆ãtÇÃéñÇÇ‚ÇÈ
	if (endFlag)
	{
		larpScale.y = 0.1f;

		float mul = 0.3f;
		//ÉEÉBÉìÉhÉEÇÃècÇã∑ÇﬂÇÈ
		KazMath::Larp(larpScale.y, &flame2DTex.data.transform.scale.y, mul);


		if (flame2DTex.data.transform.scale.y <= 0.11f)
		{
			larpScale.x = 0.0f;
		}
		//ÉEÉBÉìÉhÉEÇÃâ°Çñ≥Ç≠Ç∑
		if (flame2DTex.data.transform.scale.x <= 0.02f)
		{
			larpScale.y = 0.0f;
			endFlag = false;
			drawFlag = false;
		}
		KazMath::Larp(larpScale.x, &flame2DTex.data.transform.scale.x, mul);
	}
}

void WindowFlame::Update3D()
{
	flameTex.data.transform.pos = basePos;

	if (startFlag)
	{
		larpScale.x = lerpMaxScale.x;

		float mul = 0.3f;
		//ÉEÉBÉìÉhÉEÇâ°Ç…çLÇ∞ÇÈ
		KazMath::Larp(larpScale.x, &flameTex.data.transform.scale.x, mul);


		if (lerpMaxScale.x - 0.1f <= flameTex.data.transform.scale.x)
		{
			flameTex.data.transform.scale.x = lerpMaxScale.x;
		}
		//ÉEÉBÉìÉhÉEÇècÇ…çLÇ∞ÇÈ
		if (larpScale.x <= flameTex.data.transform.scale.x)
		{
			larpScale.y = lerpMaxScale.y;
		}
		KazMath::Larp(larpScale.y, &flameTex.data.transform.scale.y, mul);

		//ècÇ…çLÇ™Ç¡ÇΩ
		if (lerpMaxScale.y - 0.09f <= flameTex.data.transform.scale.y)
		{
			startFlag = false;
		}
	}


	//ÇµÇŒÇÁÇ≠ï`âÊÇµÇΩÇÁìoèÍââèoÇ∆ãtÇÃéñÇÇ‚ÇÈ
	if (endFlag)
	{
		larpScale.y = 0.1f;

		float mul = 0.3f;
		//ÉEÉBÉìÉhÉEÇÃècÇã∑ÇﬂÇÈ
		KazMath::Larp(larpScale.y, &flameTex.data.transform.scale.y, mul);


		if (flameTex.data.transform.scale.y <= 0.11f)
		{
			//flameTex.data.transform.scale.y = 0.1f;
			larpScale.x = 0.0f;
		}
		//ÉEÉBÉìÉhÉEÇÃâ°Çñ≥Ç≠Ç∑
		if (flameTex.data.transform.scale.x <= 0.0f)
		{
			larpScale.y = 0.0f;
			drawFlag = false;
		}
		KazMath::Larp(larpScale.x, &flameTex.data.transform.scale.x, mul);
	}
}
