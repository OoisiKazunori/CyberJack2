#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../Game/Interface/IOperationObject.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Event/GoalBoxEffect.h"
#include"../Game/Event/GoalBoxPortalEffect.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../Game/Event/PortalFlame.h"
#include<array>

class GoalBox :public IOperationObject
{
public:
	GoalBox();

	void Init(const KazMath::Vec3<float> &POS);
	void Update();
	void Draw();
	
	void Appear(const KazMath::Vec3<float> &POS);

	//ゴールエフェクト
	GoalBoxEffect lightEffect;
	Sphere hitBox;
	bool releaseFlag;
	bool startPortalEffectFlag;

private:
	static const int HP = 8;

	//移動-----------------------
	KazMath::Vec3<float> lerpRota;//角度のラープ
	KazMath::Vec3<float> lerpScale;//角度のラープ
	KazMath::Vec3<float> lerpPos; //座標のラープ
	KazMath::Vec3<float> moveVel; //当たった際にどれくらい移動するか
	KazMath::Vec3<float> moveRotaVel;//当たった際にどれくらいまわるか
	float lerpMoveVel;		//ラープの速度
	float lerpMoveRotaVel;	//ラープの速度

	KazMath::Vec3<float> goCenterPos;//全弾ヒット時にゴール地点に向かう


	int prevHpNum;//前フレームの体力
	//描画-----------------------
	ObjModelRenderPtr model;

	float addRota;
	float addVel;
	bool hitFlag;

	int intervalTimer;

	RESOURCE_HANDLE damageSoundHandle;
	RESOURCE_HANDLE warpSoundHandle;
	bool initWarpSoundFlag;

};