#pragma once
#include"../DirectXCommon/Base.h"
#include"../Math/KazMath.h"
#include"../Helper/OutPutDebugStringAndCheckResult.h"
#include"../KazLibrary/Loader/TextureResourceMgr.h"
#include"../KazLibrary/Loader/FbxModelResourceMgr.h"
#include"KazRenderHelper.h"
#include<vector>
#include<array>

namespace KazHelper
{
	/// <summary>
	/// 数値が配列の最大を超えるかどうか調べます
	/// </summary>
	/// <param name="NUM">調べたい数値</param>
	/// <param name="MAX_SIZE">配列の最大数</param>
	/// <returns>true...その数値は0~配列の最大数の内に入ります。false...その数値は0~配列の最大数の内に入りません</returns>
	inline bool IsitInAnArray(int NUM, size_t MAX_SIZE)
	{
		if (0 <= NUM && NUM < MAX_SIZE)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/// <summary>
	/// string型の文字列をwchar_t型の文字列に変換します
	/// </summary>
	/// <param name="STRING">string型の文字列</param>
	/// <param name="WCHAR_STRING">wchar_t型の文字列</param>
	/// <param name="ARRAY_SIZE">wchar_t型の文字列の最大の要素数</param>
	void ConvertStringToWchar_t(std::string STRING, wchar_t *WCHAR_STRING, size_t ARRAY_SIZE);

	/// <summary>
	/// 毎フレーム値が変わったかどうか確認します
	/// </summary>
	/// <param name="VALUE">現在の値</param>
	/// <param name="OLD_VALUE">1Flame前の値</param>
	/// <returns>true...変化があった,false...変化がなかった</returns>
	bool DirtyFlag(DirectX::XMFLOAT3 VALUE, DirectX::XMFLOAT3 OLD_VALUE);
	bool DirtyFlag(DirectX::XMFLOAT2 VALUE, DirectX::XMFLOAT2 OLD_VALUE);
	bool DirtyFlag(float VALUE, float OLD_VALUE);


	/// <summary>
	/// 現在のワールド座標が画面内にあるかどうか見ます
	/// </summary>
	/// <param name="POS">調べる座標</param>
	/// <returns>true...画面内,false...画面外</returns>
	bool IsInScreen(const KazMath::Vec3<float> &POS);

	KazMath::Vec3<float> GetScreenPos(const KazMath::Vec3<float> &POS, const KazMath::Vec2<float> &LEFTUP_POS = { 0.0f,0.0f });

	KazMath::Vec3<float> GetRightDownStagePos(const KazMath::Vec3<float> &POS, const KazMath::Vec2<float> &RIGHT_POS = { static_cast<float>(WIN_X) - 400.0f,static_cast<float>(WIN_Y) });


	int GetDigit(int NUM);

	std::vector<int> CountNumber(int TIME, int ZERO = -1);


	std::vector<char>CovertStringToChar(std::string STRING);



}


