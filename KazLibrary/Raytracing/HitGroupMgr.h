#pragma once
#include <memory>
#include <vector>
#include <string>
#include <array>
#include <wtypes.h>
#include "../Helper/ISinglton.h"

namespace Raytracing {

	class HitGroup;
	class RayRootsignature;

	/// <summary>
	/// HitGroupを管理、保存するクラス。現在は1つのHitGroupしか無いが、増やす予定です。
	/// </summary>
	class HitGroupMgr : public ISingleton<HitGroupMgr>{

	private:

		/*===== メンバ変数 =====*/

		std::vector<std::shared_ptr<HitGroup>> m_hitGroup;	//ヒットグループ


	public:

		/// <summary>
		/// ヒットグループ識別ID
		/// </summary>
		enum HITGROUP_ID {

			DEF,
			MAX_HIT_GROUP,

		};

		/// <summary>
		/// ヒットグループ識別ID、文字列
		/// </summary>
		::std::vector<LPCWSTR> hitGroupNames;


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// セッティング処理
		/// </summary>
		void Setting();

		/// <summary>
		/// 配列数のゲッタ
		/// </summary>
		/// <returns> ヒットグループの数 </returns>
		int GetHitGroupCount() { return static_cast<int>(m_hitGroup.size()); }

		//CH関連のゲッタ
		const LPCWSTR& GetCH(HITGROUP_ID arg_index);
		bool GetCHFlag(HITGROUP_ID arg_index);
		//AH関連のゲッタ
		const LPCWSTR& GetAH(HITGROUP_ID arg_index);
		bool GetAHFlag(HITGROUP_ID arg_index);
		//IS関連のゲッタ
		const LPCWSTR& GetIS(HITGROUP_ID arg_index);
		bool GetISFlag(HITGROUP_ID arg_index);
		//各種要素数のゲッタ
		int GetSRVCount(HITGROUP_ID arg_index);
		int GetCBVCount(HITGROUP_ID arg_index);
		int GetUAVCount(HITGROUP_ID arg_index);
		//ローカルルートシグネチャのゲッタ
		const ::std::shared_ptr<RayRootsignature> GetLocalRootSig(HITGROUP_ID arg_index);
		//ヒットグループ名からヒットグループIDを取得する。
		int GetHitGroupID(const LPCWSTR& arg_hitGroupName);

		//ヒットグループのSRVの数を取得する。
		int GetHitGroupSRVCount(HITGROUP_ID arg_hitGroupID);
		//ヒットグループのUAVの数を取得する。
		int GetHitGroupUAVCount(HITGROUP_ID arg_hitGroupID);

	};

}