#pragma once
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d12.h>

namespace Raytracing
{

	class Blas;

	//BottomLevelAccelerationStructureに必要なデータをまとめたクラス
	//レイトレで描画するオブジェクトのBLASを毎フレームこれに登録する。
	class BlasVector {

	private:

		/*===== メンバ変数 =====*/

		std::vector<std::weak_ptr<Blas>> m_refBlas;	//レイトレで描画するBlasの参照 (毎フレーム登録する。)
		std::vector<D3D12_RAYTRACING_INSTANCE_DESC> m_instanceDesc;	//BlasをTlasに登録するために変形した状態。


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// コンストラクタ
		/// </summary>
		BlasVector();

		/// <summary>
		/// 更新処理 主に配列を空にする。
		/// </summary>
		void Update();

		/// <summary>
		/// レイトレとして描画するために配列に追加。
		/// </summary>
		/// <param name="arg_refBlas"></param>
		/// <param name="arg_worldMat"></param>
		void Add(std::weak_ptr<Blas> arg_refBlas, const DirectX::XMMATRIX& arg_worldMat, int arg_instanceIndex = 0);
		void AddVector(std::weak_ptr<Blas> arg_refBlas, std::vector<DirectX::XMMATRIX> arg_worldMat, int arg_instanceIndex = 0, UINT arg_instanceMask = 0xFF);

		/// <summary>
		/// 保存されている参照の数。
		/// </summary>
		int GetBlasRefCount();
		int GetInstanceCount();

		/// <summary> 
		/// 参照元のシェーダーレコードにアクセスして書き込む。
		/// </summary>
		/// <param name="arg_dest"> 書き込み先ポインタ </param>
		/// <param name="arg_recordSize"> 書き込むサイズ </param>
		/// <param name="arg_stateObject"> 書き込むオブジェクト </param>
		/// <param name="arg_hitGroup"> 使用するHitGroupの名前 </param>
		/// <returns> 書き込み後のポインタ </returns>
		uint8_t* WriteShaderRecord(uint8_t* arg_dest, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stateObject, LPCWSTR arg_hitGroup);

		/// <summary>
		/// Instance配列の先頭アドレスを返す。
		/// </summary>
		/// <returns></returns>
		inline void* GetInstanceData() { return m_instanceDesc.data(); }

	};

}