#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Event/GoalBoxPortalEffect.h"
#include"../Game/Event/PortalFlame.h"

class PortalEffect
{
public:
	PortalEffect();

	void Init(const KazMath::Vec3<float> &POS);
	void Update();
	void DrawFlame();
	void DrawPortal();

	void Start();
	bool AllHidden();
	bool DrawPrevStageFlag();
	bool Reset();
	void CheckCameraPos(float POS_Z);

	int timer;
	RESOURCE_HANDLE portalTexHandle;
private:
	GoalBoxPortalEffect portalEffect;
	bool initwarpFlameFlag;
	float playerCameraPosZ;
	PortalFlame portalFlame;

};
