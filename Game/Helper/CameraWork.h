#pragma once
#include"../KazLibrary/Math/KazMath.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/Helper/EnemyHelper.h"

class CameraWork
{
public:
	CameraWork();
	void Init(const KazMath::Vec3<float> &BASE_POS = { 0.0f,3.0f,0.0f });
	void Update(const KazMath::Vec2<float> &CURSOR_VALUE, KazMath::Vec3<float> *PLAYER_POS, bool DEBUG_FLAG);

	void ForceCamera(KazEnemyHelper::CameraDir DIR);

	const KazMath::Vec3<float> GetEyePos();
	const KazMath::Vec3<float> GetTargetPos();

private:

	KazMath::Vec3<float> debugCameraMove;
	KazMath::Vec3<float> eyePos, targetPos;
	KazMath::Vec2<float> angle;

	KazMath::Vec3<float> baseTargetPos;
	KazMath::Vec2<float> leftRightAngleVel;					//左右視点座標の向く角度
	KazMath::Vec2<float> upDownAngleVel;					//上下視点座標の向く角度
	KazMath::Vec2<float> trackLeftRightAngleVel;
	KazMath::Vec2<float> trackUpDownAngleVel;
	KazMath::Vec2<float> forceCameraDirVel;					//カメラの前後左右の向きを強制的に指定する
	KazMath::Vec2<float> mulValue;							//カメラの上下左右の掛ける割合
	KazMath::Vec2<float> mulValue2;							//カメラの上下左右の掛ける割合

	KazMath::Vec3<float> honraiPlayerCameraPos;

	const float FORCE_CAMERA_FRONT = -90.0f;
	const float FORCE_CAMERA_BACK = -270.0f;
	const float FORCE_CAMERA_LEFT = 0.0f;
	const float FORCE_CAMERA_RIGHT = -180.0f;

	//カメラ挙動の確認
	BoxPolygonRenderPtr besidePoly, verticlaPoly, cameraPoly;
	KazMath::Vec3<float> centralPos;		//左右回転の中心座標
	KazMath::Vec3<float> centralPos2;		//上下回転の中心座標
	float r;					//左右回転の円の大きさ
	float r2;					//上下回転の円の大きさ

	KazMath::Vec3<float> forceCameraAngle;	//カメラを強制的に他の方向に向かせる際に使用する値
};