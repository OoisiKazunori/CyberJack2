#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Collision/CollisionManager.h"
#include"../KazLibrary/Render/KazRender.h"
#include"../Game/UI/LockOnBoxEffect.h"

class Cursor
{
public:
	Cursor();

	void Init();
	void Input(bool UP_FLAG, bool DOWN_FLAG, bool LEFT_FLAG, bool RIGHT_FLAG, bool DONE_FLAG, bool RELEASE_FLAG, const KazMath::Vec2<float> &ANGLE);
	void Update();
	void Draw();

	/// <summary>
	/// ロックオン数が最大値未満であること、ロックオンが有効でロックオンの入力がされている
	/// 時にtrueが変える
	/// </summary>
	/// <returns></returns>
	bool LockOn();
	bool Release();

	void Hit(KazMath::Vec3<float> *POS);
	const int &GetCount();

	KazMath::Vec2<float> GetValue();

	void Disappear();
	void Appear();

	/// <summary>
	/// ロックオン数を減らす
	/// </summary>
	/// <param name="SUB_VALUE">減らす分</param>
	void SubCount(int SUB_VALUE);

	KazMath::Vec2<float> honraiCursorPos;
	KazMath::Vec2<float> cursorPos;
	KazMath::Vec2<float> prevCursorPos;
	KazMath::Vec2<float> vel;
	Ray hitBox;
	DirectX::XMINT2 stopFlag;
	bool releaseFlag;


	KazMath::Vec2<float> honraiCameraMoveValue;//上下左右にカメラをどれくらい動かすかの値を保存する(本来ポズ)
	KazMath::Vec2<float> cameraMoveValue;//上下左右にカメラをどれくらい動かすかの値を保存する
	KazMath::Vec2<float> dontMoveCameraStartPos;//カメラの無操作開始地点
	KazMath::Vec2<float> dontMoveCameraEndPos;//カメラの無操作終了地点

	KazMath::Vec2<float> limitValue;//カメラの端の値

	static KazMath::Vec2<float> NO_MOVE_DISTANCE;//XYそれぞれどこまでの範囲ならカーソル動かしてもカメラに影響が出ないか

private:
	//ロックオン数----------------------------------------------------------------
	static const int LOCKON_MAX_NUM = 8;	//最大ロックオン数
	int lockOnNum;							//現在のロックオン数
	bool notEnableLockOnFlag;
	int enableLockOnTimer;

	//移動----------------------------------------------------------------
	bool upFlag, downFlag, leftFlag, rightFlag,doneFlag;
	KazMath::Vec2<float> stickAngle;
	float deadLine;
	float baseSpeed;
	DirectX::XMFLOAT2 speed;

	//入力演出-----------------------

	struct CursorEffectData
	{
		bool initFlag;
		Sprite2DRenderPtr cursorEffectTex;

		CursorEffectData() :cursorEffectTex(std::make_unique<Sprite2DRender>()), initFlag(false)
		{
		}
	};

	std::array<CursorEffectData, 10> cursorEffectTex;


	//操作無効----------------------------------------------
	bool dontMoveFlag;	//カメラが固定になった際にカーソルを動かさない処理
	int cursorAlpha;	//カーソルの透明度
	bool disappearFlag;//カーソルを消すフラグ
	//操作無効----------------------------------------------


	//画像----------------------------------------------------------------
	Sprite2DRenderPtr cursorFlameTex, numberTex;	//スクリーン座標の描画
	std::array<RESOURCE_HANDLE, LOCKON_MAX_NUM + 1> numberHandle;			//数字用の画像ハンドル
	RESOURCE_HANDLE flameHandle;
	KazMath::Vec2<float> baseScale;

	KazMath::Vec2<float> oldPos;
	KazMath::Vec2<float> oldVel;


	//ノックバック挙動-----------------------
	KazMath::Vec2<float> knockBackVal;	//反転移動用の移動量
	static DirectX::XMFLOAT2 KOCKBACK_MAX_VALUE;		//ノックバックの最大量(値が決まったらconstにする)
	static DirectX::XMFLOAT2 KOCKBACK_VELOCITY;		//ノックバックの加算量(値が決まったらconstにする)
	DirectX::XMINT2 noTochFlag;
	DirectX::XMINT2 noTockTimer;
	//ノックバック挙動-----------------------

	int clickSoundHandle;
	bool initClickSoundFlag;


	//ロックオン演出
	std::array<LockOnBoxEffect, LOCKON_MAX_NUM * 2> boxEffectArray;

};

