#include "PortalEffect.h"

PortalEffect::PortalEffect()
{
}

void PortalEffect::Init(const KazMath::Vec3<float> &POS)
{
	KazMath::Vec3<float> adj = { 0.0f,0.0f,45.0f };
	portalEffect.Init(POS + adj);
	portalFlame.Init(POS + adj, KazMath::Vec2<float>(41.5f, 23.5f));
	initwarpFlameFlag = false;
}

void PortalEffect::Update()
{
	if (portalFlame.Flame())
	{
		portalEffect.Start();
	}

	/*if (20 < portalEffect.timer && !initWarpSoundFlag)
	{
		SoundManager::Instance()->PlaySoundMem(warpSoundHandle, 1);
		initWarpSoundFlag = true;
	}*/

	float lAdjPos = 0.0f;
	if (portalFlame.basePos.z < playerCameraPosZ + 1.0f && !initwarpFlameFlag)
	{
		portalFlame.basePos.z -= 0.5f;
		portalEffect.changeStageFlag = true;
		initwarpFlameFlag = true;
		lAdjPos = -0.5f;
	}
	else if(initwarpFlameFlag)
	{
		lAdjPos = -0.5f;
	}

	portalEffect.noiseSprite->data.transform.pos.z = portalFlame.basePos.z + lAdjPos;
	portalEffect.noiseSprite->data.handleData = portalTexHandle;

	portalEffect.Update();
	portalFlame.Update();
}

void PortalEffect::DrawFlame()
{
	portalFlame.Draw();
}

void PortalEffect::DrawPortal()
{
	portalEffect.Draw();
}

void PortalEffect::Start()
{
	portalFlame.Start();
}

bool PortalEffect::AllHidden()
{
	return portalEffect.AllHidden();
}

bool PortalEffect::DrawPrevStageFlag()
{
	return initwarpFlameFlag;
}

bool PortalEffect::Reset()
{
	return portalEffect.noiseSprite->data.transform.pos.z <= -20.0f;
}

void PortalEffect::CheckCameraPos(float POS_Z)
{
	playerCameraPosZ = POS_Z;
	portalEffect.CheckCameraPos(POS_Z);
}
