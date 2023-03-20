#pragma once
#include"../DirectXCommon/Base.h"

/// <summary>
/// 名前を登録するクラス
/// </summary>
class RegisterNameWithData
{
public:
	RegisterNameWithData();
	~RegisterNameWithData();

	/// <summary>
	/// 名前を登録します
	/// 同じ名前があった場合は登録されずに出力に警告文が書かれます</summary>
	/// <param name="NAME">登録したい名前</param>
	/// <returns>true...登録に成功しました　false...登録に失敗しました</returns>
	bool RegisterName(std::string NAME);

	/// <summary>
	/// 登録した名前を削除します
	/// 削除したい名前が無かった場合は出力に警告文が書かれます
	/// </summary>
	/// <param name="NAME">削除したい名前</param>
	bool UnRegisterName(std::string NAME);

	/// <summary>
	/// 名前があるかどうか確認します
	/// </summary>
	/// <param name="NANME">確認したい名前</param>
	/// <returns>0以上の値...名前を発見しました。何番目の値か渡します　-1...名前は見つかりませんでした</returns>
	int IsNameExists(std::string NAME);

	/// <summary>
	/// ハンドルから登録した名前を削除します
	/// </summary>
	/// <param name="HANDLE">削除したいハンドル</param>
	void UnRegisterName(int HANDLE);

private:
	std::vector<std::string> name;
};

