#pragma once
#include <array>
#include <wrl.h>
#include "../DirectXCommon/d3dx12.h"

namespace Raytracing {

	/// <summary>
	/// レイトレーシングで使用するグローバルルートシグネチャ、ローカルルートシグネチャクラス
	/// </summary>
	class RayRootsignature {

	private:

		/*===== 定数 =====*/

		static const int MAX_ROOTPARAM = 16;
		static const int MAX_SAMPLER = 26;


	private:

		/*===== メンバ変数 =====*/

		::std::array<CD3DX12_ROOT_PARAMETER, MAX_ROOTPARAM> m_rootparam;	//ルートパラメーター
		::std::array<CD3DX12_DESCRIPTOR_RANGE, MAX_ROOTPARAM> m_descRange;//ディスクリプタテーブル
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootsignature;	//ルートシグネチャ
		::std::array<CD3DX12_STATIC_SAMPLER_DESC, MAX_SAMPLER> m_sampler;	//スタティックサンプラー
		UINT m_rootparamCount;											//ルートパラメーター数
		UINT m_samplerCount;											//サンプラーの数


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// コンストラクタ
		/// </summary>
		RayRootsignature() {
			m_rootparamCount = 0;
			m_samplerCount = 0;
		}

		/// <summary>
		/// ルートパラメーター追加処理
		/// </summary>
		/// <param name="arg_type"> データの種類 SRVとかUAVとか </param>
		/// <param name="arg_shaderRegister"> レジスタ番号 t0とかの0の方 </param>
		/// <param name="arg_registerSpace"> レジスタスペース t0の後にくるspaceの数字 </param>
		void AddRootparam(D3D12_DESCRIPTOR_RANGE_TYPE arg_type, UINT arg_shaderRegister, UINT arg_registerSpace = 0);

		/// <summary>
		/// スタティックサンプラー追加処理
		/// </summary>
		/// <param name="RegisterSpace"> レジスタスペース t0の後にくるspaceの数字 </param>
		void AddStaticSampler(int arg_registerSpace = 0);

		/// <summary>
		/// ルートシグネチャの生成
		/// </summary>
		/// <param name="IsLocal"> ローカルルートシグネチャかどうか </param>
		/// <param name="Name"> バッファの名前 </param>
		void Build(bool arg_isLocal, const wchar_t* arg_name = nullptr);

		//ゲッタ
		inline Microsoft::WRL::ComPtr<ID3D12RootSignature>& GetRootSig() { return m_rootsignature; }

	};

}