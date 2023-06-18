#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <array>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "HitGroupMgr.h"

namespace Raytracing {

	class RayRootsignature;
	class RaytracingOutput;

	/// <summary>
	/// シェーダーテーブルに登録する各種シェーダーのエントリポイント名を保存したもの。
	/// </summary>
	struct RayPipelineShaderData {

		std::string shaderPath_;					// シェーダーパス
		std::vector<LPCWSTR> rayGenEnteryPoint_;	// エントリポイント
		std::vector<LPCWSTR> missEntryPoint_;		// エントリポイント
		std::vector<LPCWSTR> hitgroupEntryPoint_;	// エントリポイント
		RayPipelineShaderData() {};
		RayPipelineShaderData(const std::string& ShaderPath, const std::vector<LPCWSTR>& RGEntry, const std::vector<LPCWSTR>& MSEntry, const std::vector<LPCWSTR>& HGEntry)
			:shaderPath_(ShaderPath), rayGenEnteryPoint_(RGEntry), missEntryPoint_(MSEntry), hitgroupEntryPoint_(HGEntry) {};

	};

	/// <summary>
	/// レイトレーシングで使用するパイプライン
	/// </summary>
	class RayPipeline {

	private:

		/*===== メンバ変数 =====*/

		std::vector<RayPipelineShaderData> shaderData_;			// 使用するシェーダーを纏めた構造体
		std::vector<D3D12_SHADER_BYTECODE> shaderCode_;			// 使用するシェーダーのバイトコード
		Microsoft::WRL::ComPtr<ID3D12StateObject> stateObject_;	// ステートオブジェクト
		std::shared_ptr<RayRootsignature> globalRootSig_;		// グローバルルートシグネチャ
		D3D12_DISPATCH_RAYS_DESC dispatchRayDesc_;				// レイ発射時の設定
		Microsoft::WRL::ComPtr<ID3D12Resource> shaderTable_;	// シェーダーテーブル
		void* shaderTalbeMapAddress_;							// シェーダーテーブルのデータ転送用Mapアドレス
		Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> rtsoProps_;
		LPCWSTR hitGroupName_;
		int numBlas_;											// パイプラインに組み込まれているBlasの数。BlasRefecenceVectorが保持している参照の数がこの変数の数を超えたらパイプラインを作り直す。


		// シェーダーテーブルの構築に必要な変数群
		UINT raygenRecordSize_;
		UINT missRecordSize_;
		UINT hitgroupRecordSize_;
		UINT hitgroupCount;
		UINT raygenSize_;
		UINT missSize_;
		UINT hitGroupSize_;
		UINT tableAlign_;
		UINT hitgroupRegion_;
		UINT tableSize_;
		UINT raygenRegion_;
		UINT missRegion_;


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="InputData"> 使用するシェーダーの各種データ </param>
		/// <param name="UseHitGroup"> 使用するHitGroupの名前 </param>
		/// <param name="SRVCount"> 使用するSRVの数 </param>
		/// <param name="CBVCount"> 使用するCBVの数 </param>
		/// <param name="UAVCount"> 使用するUAVの数 </param>
		/// <param name="PayloadSize"> シェーダーで使用するペイロードのサイズ </param>
		/// <param name="AttribSize"> 重心座標などの計算に使用する構造体のサイズ 固定 </param>
		/// <param name="ReflectionCount"> 最大レイ射出数 </param>
		RayPipeline(const std::vector<RayPipelineShaderData>& InputData, HitGroupMgr::HITGROUP_ID UseHitGroup, int SRVCount, int CBVCount, int UAVCount, int PayloadSize, int AttribSize, int ReflectionCount = 4);

		/// <summary>
		/// シェーダーテーブルを構築。 毎フレーム全てのDrawが終わったときに呼ぶ。
		/// </summary>
		/// <param name="DispatchX"> 横の画面サイズ </param>
		/// <param name="DispatchY"> 縦の画面サイズ </param>
		void BuildShaderTable(int DispatchX = 1280, int DispatchY = 720);

		/// <summary>
		/// シェーダーテーブルを構築。
		/// </summary>
		/// <param name="DispatchX"> 横の画面サイズ </param>
		/// <param name="DispatchY"> 縦の画面サイズ </param>
		void ConstructionShaderTable(int DispatchX, int DispatchY);

		/// <summary>
		/// HipGroupのデータをGPUに転送。 Blasのデータなどを送る。
		/// </summary>
		void MapHitGroupInfo();

		/// <summary>
		/// レイトレ実行。
		/// </summary>
		void TraceRay(std::weak_ptr<RaytracingOutput> Output, std::weak_ptr<RaytracingOutput> GBuffer0, std::weak_ptr<RaytracingOutput> GBuffer1, std::weak_ptr<RaytracingOutput> RenderUAV);

		// 各種ゲッタ
		Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return stateObject_; }
		D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return dispatchRayDesc_; }
		std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return globalRootSig_; }

	private:

		/// <summary>
		/// ヒットグループの中で最も使用データサイズが大きものを取得する。
		/// </summary>
		/// <returns> ヒットグループの中で最も大きいサイズ </returns>
		UINT GetLargestDataSizeInHitGroup();

		/// <summary>
		/// アラインメントする。
		/// </summary>
		/// <param name="Size"> アラインメントしたいサイズ。 </param>
		/// <param name="Align"> アラインメント </param>
		/// <returns> 結果 </returns>
		UINT RoundUp(size_t Size, UINT Align) {
			return UINT(Size + Align - 1) & ~(Align - 1);
		}

		/// <summary>
		/// シェーダーテーブルを書き込み、レイを設定する。--------------------------------------------------------------------------シェーダーテーブルの書き込みとレイの設定の関数を分ける。
		/// </summary>
		/// <param name="DispatchX"></param>
		/// <param name="DispatchY"></param>
		void WriteShadetTalbeAndSettingRay(int DispatchX, int DispatchY);

		/// <summary>
		/// シェーダー識別子を書き込む。
		/// </summary>
		/// <param name="Dst"> 書き込み先 </param>
		/// <param name="ShaderId"> 書き込むID </param>
		/// <returns> 書き込んだ後のハンドル </returns>
		UINT WriteShaderIdentifier(void* Dst, const void* ShaderId);

		/// <summary>
		/// RayGenerationシェーダーの数を取得。
		/// </summary>
		/// <returns></returns>
		int GetRayGenerationCount();

		/// <summary>
		///  MissShaderの数を取得。
		/// </summary>
		/// <returns></returns>
		int GetMissCount();

		/// <summary>
		/// バッファを生成する関数 面倒だったので私のエンジンからそのまま持ってきました。
		/// </summary>
		/// <param name="Size"> バッファサイズ </param>
		/// <param name="Flags"> バッファのフラグ </param>
		/// <param name="InitialState"> バッファの初期状態 </param>
		/// <param name="HeapType"> ヒープの種類 </param>
		/// <param name="Name"> バッファにつける名前 </param>
		/// <returns> 生成されたバッファ </returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t Size, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES InitialState, D3D12_HEAP_TYPE HeapType, const wchar_t* Name = nullptr);

	};

}