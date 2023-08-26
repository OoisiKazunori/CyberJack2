#pragma once
#include <string>
#include <wtypes.h>
#include <vector>
#include <array>
#include <d3d12.h>
#include <wrl.h>
#include <memory>
#include "BlasVector.h"
#include "Tlas.h"
#include "HitGroupMgr.h"
#include "DirectXCommon/DirectX12.h"
#include "Helper/KazBufferHelper.h"
#include "Helper/Compute.h"

namespace PostEffect {
	class LensFlare;
}

namespace Raytracing {

	class RayRootsignature;
	//class RaytracingOutput;

	/// <summary>
	/// シェーダーテーブルに登録する各種シェーダーのエントリポイント名を保存したもの。
	/// </summary>
	struct RayPipelineShaderData {

		std::string m_shaderPath;					//シェーダーパス
		std::vector<LPCWSTR> m_rayGenEntryPoint;	//エントリポイント
		std::vector<LPCWSTR> m_missEntryPoint;		//エントリポイント
		std::vector<LPCWSTR> m_hitgroupEntryPoint;	//エントリポイント
		RayPipelineShaderData() {};
		RayPipelineShaderData(const std::string& arg_shaderPath, const std::vector<LPCWSTR>& arg_RGEntry, const std::vector<LPCWSTR>& arg_MSEntry, const std::vector<LPCWSTR>& arg_HGEntry)
			:m_shaderPath(arg_shaderPath), m_rayGenEntryPoint(arg_RGEntry), m_missEntryPoint(arg_MSEntry), m_hitgroupEntryPoint(arg_HGEntry) {};

	};

	/// <summary>
	/// レイトレーシングで使用するパイプライン
	/// </summary>
	class RayPipeline {

	private:

		/*===== メンバ変数 =====*/

		std::vector<RayPipelineShaderData> m_shaderData;		//使用するシェーダーを纏めた構造体
		std::vector<D3D12_SHADER_BYTECODE> m_shaderCode;		//使用するシェーダーのバイトコード
		Microsoft::WRL::ComPtr<ID3D12StateObject> m_stateObject;//ステートオブジェクト
		std::shared_ptr<RayRootsignature> m_globalRootSig;		//グローバルルートシグネチャ
		D3D12_DISPATCH_RAYS_DESC m_dispatchRayDesc;				//レイ発射時の設定
		Microsoft::WRL::ComPtr<ID3D12Resource> m_shaderTable;	//シェーダーテーブル
		void* m_shaderTalbeMapAddress;							//シェーダーテーブルのデータ転送用Mapアドレス
		Microsoft::WRL::ComPtr<ID3D12StateObjectProperties> m_rtsoProps;
		LPCWSTR m_hitGroupName;
		int m_numBlas;											//パイプラインに組み込まれているBlasの数。BlasRefecenceVectorが保持している参照の数がこの変数の数を超えたらパイプラインを作り直す。


		//シェーダーテーブルの構築に必要な変数群
		UINT m_raygenRecordSize;
		UINT m_missRecordSize;
		UINT m_hitgroupRecordSize;
		UINT m_hitgroupCount;
		UINT m_raygenSize;
		UINT m_missSize;
		UINT m_hitGroupSize;
		UINT m_tableAlign;
		UINT m_hitgroupRegion;
		UINT m_tableSize;
		UINT m_raygenRegion;
		UINT m_missRegion;

		static DirectX12* m_refDirectX12;

		KazBufferHelper::BufferData* m_refVolumeNoiseTexture;
		KazBufferHelper::BufferData* m_refRaymarchingConstData;
		KazBufferHelper::BufferData* m_refDebugOnOffConstData;
		KazBufferHelper::BufferData* m_refDebugSeaConstData;

		//レンズフレアクラス
		std::shared_ptr<PostEffect::LensFlare> m_lensFlare;

		//アウトライン合成用
		ComputeShader m_outlineComposeShader;
		ComputeShader m_outlineCleanShader;

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
		RayPipeline(const std::vector<RayPipelineShaderData>& arg_inputData, HitGroupMgr::HITGROUP_ID arg_useHitGroup, int arg_SRVCount, int arg_CBVCount, int arg_UAVCount, int arg_payloadSize, int arg_attribSize, int arg_reflectionCount = 4);

		/// <summary>
		/// シェーダーテーブルを構築。 毎フレーム全てのDrawが終わったときに呼ぶ。
		/// </summary>
		/// <param name="arg_blacVector"> Blasの配列 </param>
		/// <param name="arg_dispatchX"> 横の画面サイズ </param>
		/// <param name="arg_dispatchY"> 縦の画面サイズ </param>
		void BuildShaderTable(BlasVector arg_blacVector, int arg_dispatchX = 1280, int arg_dispatchY = 720);

		/// <summary>
		/// シェーダーテーブルを構築。
		/// </summary>
		/// <param name="arg_blacVector"> Blasの配列 </param>
		/// <param name="arg_dispatchX"> 横の画面サイズ </param>
		/// <param name="arg_dispatchY"> 縦の画面サイズ </param>
		void ConstructionShaderTable(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY);

		/// <summary>
		/// HipGroupのデータをGPUに転送。 Blasのデータなどを送る。
		/// </summary>
		/// <param name="arg_blacVector"> Blasの配列 </param>
		void MapHitGroupInfo(BlasVector arg_blacVector);

		/// <summary>
		/// レイトレ実行。
		/// </summary>
		void TraceRay(Tlas arg_tlas);

		//各種ゲッタ
		Microsoft::WRL::ComPtr<ID3D12StateObject> GetStateObject() { return m_stateObject; }
		D3D12_DISPATCH_RAYS_DESC GetDispatchRayDesc() { return m_dispatchRayDesc; }
		std::shared_ptr<RayRootsignature> GetGlobalRootSig() { return m_globalRootSig; }

		static void SetDirectX12(DirectX12* arg_refDirectX12) { m_refDirectX12 = arg_refDirectX12; }
		void SetVolumeFogTexture(KazBufferHelper::BufferData* arg_volumeFogTexture) { m_refVolumeNoiseTexture = arg_volumeFogTexture; }
		void SetRaymarchingConstData(KazBufferHelper::BufferData* arg_raymarchingParamConstData) { m_refRaymarchingConstData = arg_raymarchingParamConstData; }
		void SetDebugOnOffConstData(KazBufferHelper::BufferData* arg_debugOnOffConstData) { m_refDebugOnOffConstData = arg_debugOnOffConstData; }
		void SetDebugSeaConstData(KazBufferHelper::BufferData* arg_debugSeaConstData) { m_refDebugSeaConstData = arg_debugSeaConstData; }

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
		UINT RoundUp(size_t arg_aize, UINT arg_align) {
			return UINT(arg_aize + arg_align - 1) & ~(arg_align - 1);
		}

		/// <summary>
		/// シェーダーテーブルを書き込み、レイを設定する。--------------------------------------------------------------------------シェーダーテーブルの書き込みとレイの設定の関数を分ける。
		/// </summary>
		/// <param name="arg_blacVector"> Blasの配列 </param>
		/// <param name="arg_dispatchX"></param>
		/// <param name="arg_dispatchY"></param>
		void WriteShadetTalbeAndSettingRay(BlasVector arg_blacVector, int arg_dispatchX, int arg_dispatchY);

		/// <summary>
		/// シェーダー識別子を書き込む。
		/// </summary>
		/// <param name="Dst"> 書き込み先 </param>
		/// <param name="ShaderId"> 書き込むID </param>
		/// <returns> 書き込んだ後のハンドル </returns>
		UINT WriteShaderIdentifier(void* arg_dst, const void* arg_shaderId);

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
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initialState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_name = nullptr);

		//UAVにリソースバリアをかける。
		void UAVBarrier(std::vector<KazBufferHelper::BufferData> arg_bufferArray);

		//バッファの状態を遷移させる。
		void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

		//レイトレで使用するリソース類をセット。
		void SetRaytracingResource(Tlas arg_tlas);

	};

}