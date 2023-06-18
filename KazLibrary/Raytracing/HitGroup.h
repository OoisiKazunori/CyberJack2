#pragma once
#include <string>
#include <memory>
#include <wtypes.h>

namespace Raytracing {

	class RayRootsignature;

	/// <summary>
	/// HitGroupを生成する際に渡すデータ構造体
	/// </summary>
	struct EntryPoint {
		LPCWSTR m_entryPoint;
		bool m_isActive;
		EntryPoint() {};
		EntryPoint(LPCWSTR arg_entry, bool arg_flag) :m_entryPoint(arg_entry), m_isActive(arg_flag) {};
	};
	struct HitGroupInitData {

		EntryPoint m_CH;	//ClosestHitShader
		EntryPoint m_AH;	//AnyHitShader
		EntryPoint m_IS;	//IntersectShader
		int m_SRVcount;		//SRVの数
		int m_CBVcount;		//CBVの数
		int m_UAVcount;		//UAVの数
		HitGroupInitData() {};
	};

	/// <summary>
	/// ヒットグループクラス
	/// </summary>
	class HitGroup {

	private:

		/*===== メンバ変数 =====*/

		EntryPoint m_CH;	//ClosestHitShader
		EntryPoint m_AH;	//AnyHitShader
		EntryPoint m_IS;	//IntersectShader
		int m_SRVcount;		//SRVの数
		int m_CBVcount;		//CBVの数
		int m_UAVcount;		//UAVの数

		int m_registerSpace;	//シェーダーレジスターのレジスタースペース番号

		LPCWSTR m_hitGroupName;	//このヒットグループの名前

		std::shared_ptr<RayRootsignature> m_localRootSig;	//このヒットグループで使用するローカルルートシグネチャ


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// コンストラクタ
		/// </summary>
		HitGroup();

		/// <summary>
		/// 生成処理
		/// </summary>
		/// <param name="InputData"> HitGroupの初期化用データ </param>
		/// <param name="RegisterSpace"> レジスタスペース </param>
		/// <param name="HitGroupName"> このHitGroupの名前の文字列 </param>
		void Generate(const HitGroupInitData& arg_inputData, int arg_registerSpace, const LPCWSTR& arg_hitGroupName);

		// 各種ゲッタ
		const LPCWSTR& GetCH() { return m_CH.m_entryPoint; }
		const LPCWSTR& GetAH() { return m_AH.m_entryPoint; }
		const LPCWSTR& GetIS() { return m_IS.m_entryPoint; }
		bool GetCHFlag() { return m_CH.m_isActive; }
		bool GetAHFlag() { return m_AH.m_isActive; }
		bool GetISFlag() { return m_IS.m_isActive; }
		int GetSRVCount() { return m_SRVcount; }
		int GetCBVCount() { return m_CBVcount; }
		int GetUAVCount() { return m_UAVcount; }
		const std::shared_ptr<RayRootsignature> GetLoacalRootSig() { return m_localRootSig; };

	};

}