#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../KazLibrary/Collision/CollisionManager.h"

struct IOperationObjectData
{
	short rockOnNum;		//残りロックオン回数
	bool enableToHitFlag;	//判定が有効なのかどうか判断するフラグ
	bool initFlag;			//既に初期化が通ったか判断するフラグ
	int hp;					//OBJの体力
	bool lockOnFlag;
	std::string name;

	IOperationObjectData()
	{
		rockOnNum = -1;
		initFlag = false;
		enableToHitFlag = false;
		hp = -1;
		lockOnFlag = false;
	}

	/// <summary>
	/// 操作可能OBJがもつ基本的なデータの初期化
	/// </summary>
	/// <param name="MAX_ROCKON_NUM">ロックオンできる回数</param>
	void Init(short MAX_ROCKON_NUM, const std::string &NAME)
	{
		rockOnNum = MAX_ROCKON_NUM;
		hp = MAX_ROCKON_NUM;
		enableToHitFlag = true;
		initFlag = true;
		lockOnFlag = false;
		name = NAME;
	};
};

/// <summary>
/// プレイヤーが干渉出来るOBJの基底クラス
/// </summary>
class IOperationObject
{
public:
	virtual ~IOperationObject() {};

	IOperationObject();

	/// <summary>
	/// ロックオンの残り回数を減らします
	/// </summary>
	void Hit();

	/// <summary>
	/// 残りロックオン回数が0以下ならtrueを返す
	/// </summary>
	bool LockedOrNot();

	/// <summary>
	/// 当たり判定が有効で、残りロックオン数が1以上ならtrueを返す
	/// </summary>
	bool IsAlive();

	/// <summary>
	/// 指定した回数分HPを減らします
	/// </summary>
	/// <param name="COUNT">減らすHPの量</param>
	int ReduceHp(int COUNT);

	IOperationObjectData iOperationData;//操作可能OBJ全員が持つ基本データ

protected:
	const float limitLinePosZ = -50.0f;//当たり判定を有効にするライン
	int timer;							//連続ヒットしてHpを減らす際に間隔をあける為に使用
	int reduceHpNum;
	bool startToReduceHpFlag;

	/// <summary>
	/// 指定のZ座標より小さい場所より行こうとしたらfalseを返す
	/// </summary>
	/// <param name="POS_Z">対象のZ座標</param>
	bool EnableToHit(const KazMath::Vec3<float> &POS, const KazMath::Vec3<float> &LIMIT_MIN_POS, const KazMath::Vec3<float> &LIMIT_MAX_POS)
	{
		//有効な当たり判定の最小値
		bool lEnableToHitMinFlag = LIMIT_MIN_POS.x <= POS.x && LIMIT_MIN_POS.y <= POS.y && LIMIT_MIN_POS.z <= POS.z;
		//有効な当たり判定の最大値
		bool lEnableToHitMaxFlag = POS.x <= LIMIT_MAX_POS.x && POS.y <= LIMIT_MAX_POS.y && POS.z <= LIMIT_MAX_POS.z;

		return lEnableToHitMinFlag && lEnableToHitMaxFlag;
	};

	void LockOnWindow(const KazMath::Vec3<float> &POS);

	Sprite2DRender lockOnWindowRender;
};
