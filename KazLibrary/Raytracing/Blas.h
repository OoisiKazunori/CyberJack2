#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include<memory>

namespace Raytracing {

	/// <summary>
	/// BottomLevelの加速構造体 Blasとして最低限必要な機能をまとめたクラスで、全てのモデルクラスが本クラスを持つ。
	/// </summary>
	class Blas {

	private:

		/*===== メンバ変数 =====*/

		Microsoft::WRL::ComPtr<ID3D12Resource> m_blasBuffer;		//Blasのバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> m_scratchBuffer;		//Blasを形成する際に必要なバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> m_updateBuffer;		//Blasのアップロード用バッファ

		D3D12_RAYTRACING_GEOMETRY_DESC m_geomDesc;

		RESOURCE_HANDLE m_textureHandle;							//使用するテクスチャのアドレス
		RESOURCE_HANDLE m_vertexDataHandle;							//頂点データを保存してあるデータのハンドル
		int m_meshNumber;											//複数メッシュの描画をしている際に使用する。このBlasは頂点データのどのメッシュを参照するかの情報。
		int m_hitGroupIndex;


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// コンストラクタ
		/// </summary>
		/// <param name="arg_isOpaque"> 不透明フラグ t:不透明 f:半透明になる可能性がある </param>
		/// <param name="arg_vertexDataHandle"> Blasを形成するモデルの形状を保存してあるデータのハンドル </param>
		/// <param name="arg_textureHandle"> 使用するテクスチャのアドレス </param>
		Blas(bool arg_isOpaque, RESOURCE_HANDLE arg_vertexDataHandle, int arg_meshNumber, RESOURCE_HANDLE arg_textureHandle, int arg_hitGroupIndex);

		//更新処理
		void Update();

		/// <summary>
		/// Blasのバッファを返す。
		/// </summary>
		/// <returns> Blasのバッファ </returns>
		inline Microsoft::WRL::ComPtr<ID3D12Resource> GetBlasBuffer() { return m_blasBuffer; }

		/// <summary> 
		/// 参照元のシェーダーレコードにアクセスして書き込む。
		/// </summary>
		/// <param name="arg_dst"> 書き込み先ポインタ </param>
		/// <param name="arg_recordSize"> 書き込むサイズ </param>
		/// <param name="arg_stageObject"> 書き込むオブジェクト </param>
		/// <param name="arg_hitGroupName"> 使用するHitGroupの名前 </param>
		/// <returns> 書き込み後のポインタ </returns>
		uint8_t* WriteShaderRecord(uint8_t* arg_dst, UINT arg_recordSize, Microsoft::WRL::ComPtr<ID3D12StateObject>& arg_stageObject, LPCWSTR arg_hitGroupName);

	private:

		/// <summary>
		/// Blasの形状設定に必要な構造体を返す。
		/// </summary>
		/// <param name="arg_isOpaque"> 不透明フラグ t:不透明 f:半透明になる可能性がある </param>
		/// <returns></returns>
		D3D12_RAYTRACING_GEOMETRY_DESC GetGeometryDesc(bool arg_isOpaque);

		/// <summary>
		/// Blasを構築
		/// </summary>
		/// <param name="arg_geomDesc"> Blasの形状構造体。同クラスの関数から取得する。 </param>
		void BuildBlas(const D3D12_RAYTRACING_GEOMETRY_DESC& arg_geomDesc);

		/// <summary>
		/// バッファを生成する関数 面倒だったので私のエンジンからそのまま持ってきました。
		/// </summary>
		/// <param name="arg_size"> バッファサイズ </param>
		/// <param name="arg_flags"> バッファのフラグ </param>
		/// <param name="arg_initState"> バッファの初期状態 </param>
		/// <param name="arg_heapType"> ヒープの種類 </param>
		/// <param name="arg_bufferName"> バッファにつける名前 </param>
		/// <returns> 生成されたバッファ </returns>
		Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(size_t arg_size, D3D12_RESOURCE_FLAGS arg_flags, D3D12_RESOURCE_STATES arg_initState, D3D12_HEAP_TYPE arg_heapType, const wchar_t* arg_bufferName = nullptr);

		// GPUディスクリプタを書き込む。
		inline UINT WriteGPUDescriptor(void* arg_dst, const D3D12_GPU_DESCRIPTOR_HANDLE* arg_descriptor)
		{
			memcpy(arg_dst, arg_descriptor, sizeof(arg_descriptor));
			return static_cast<UINT>((sizeof(arg_descriptor)));
		}
		inline UINT WriteGPUVirtualAddress(void* arg_dst, const D3D12_GPU_VIRTUAL_ADDRESS* arg_descriptor)
		{
			memcpy(arg_dst, arg_descriptor, sizeof(arg_descriptor));
			return static_cast<UINT>((sizeof(arg_descriptor)));
		}

	public:

		int GetHitGroupIndex();

	};

}