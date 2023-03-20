#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/UI/PlayerHpUi.h"
#include"../Game/Effect/PlayerDamageEmitter.h"
#include"../Game/UI/DamageTextWindow.h"

class Player
{
public:
	Player();
	void Init(const KazMath::Vec3<float> &POS, bool DRAW_UI_FLAG = true, bool APPEAR_FLAG = false);
	void Finalize();
	void Input();
	void Update();
	void Draw();

	void ForceAnim(int PATTERN)
	{
		switch (PATTERN)
		{
		case 0:
			forceCameraRate = 0.0f;
			break;
		case 1:
			forceCameraRate = 0.0f;
			break;
		case 2:
			forceCameraRate = -0.5f;
			break;
		case 3:
			forceCameraRate = 0.5f;
			break;
		default:
			break;
		}
	};

	void Hit();

	bool IsAlive();
	KazMath::Vec3<float> pos;
	KazMath::Vec2<float> cameraRate;
private:

	enum { LEFT, RIGHT, HEAD };

	short hp,prevHp;
	int coolTimer;
	static const int COOL_MAX_TIME;
	bool coolTimeFlag;
	bool redFlag;
	std::array<FbxModelRender,3> fbxRender;
	bool drawHpFlag;

	PlayerHpUi hpUi;

	int damageSoundHandle;
	PlayerDamageEmitter damageEffect;
	DamageTextWindow damageWindow;


	bool leftFlag;
	bool rightFlag;
	FbxTime totalTime;

	int larpTime;
	KazMath::Vec3<float>minScale;
	KazMath::Vec3<float>adjPos;
	KazMath::Vec3<float>adjRota;
	float sinTimer;

	float forceCameraRate;
};

