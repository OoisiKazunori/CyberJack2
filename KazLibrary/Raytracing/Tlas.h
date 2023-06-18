#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "BlasVector.h"

namespace Raytracing {

	/// <summary>
	/// TopLevelの加速構造体 BlasVectorを登録することによってレイトレ空間を構築する。
	/// </summary>
	class Tlas {

	private:

		/*===== メンバ変数 =====*/

		Microsoft::WRL::ComPtr<ID3D12Resource> m_tlasBuffer;		//Tlasのバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> m_scratchBuffer;		//Tlasを形成する際に必要なバッファ
		Microsoft::WRL::ComPtr<ID3D12Resource> m_instanceDescBuffer;//インスタンスバッファ
		void* m_instanceDescMapAddress_;							//インスタンスバッファにデータを送る際のアドレス
		int m_instanceCapacity;										//現在のTlasのバッファのInstance数の許容量。 InstanceVectorの要素数がこの数を超えたら全てのバッファを再生成する。
		int m_descHeapIndex;										//Tlasを保存してあるDescriptorHeap上のインデックス


	public:

		/*===== メンバ関数 =====*/

		/// <summary>
		/// コンストラクタ
		/// </summary>
		Tlas();

		/// <summary>
		/// TLASを構築。全てのDrawコールを終えた後に呼んでください。
		/// </summary>
		/// <param name="arg_blasVector"> 使用するBlasの配列 </param>
		void Build(BlasVector arg_blasVector);

		int GetDescHeapHandle() { return m_descHeapIndex; }

	private:

		/// <summary>
		/// Tlasを構築or再構築する関数。
		/// </summary>
		/// <param name="arg_blasVector"> 使用するBlasの配列 </param>
		void ReBuildTlas(BlasVector arg_blasVector);

		/// <summary>
		/// Tlasを更新する関数。
		/// </summary>
		/// <param name="arg_blasVector"> 使用するBlasの配列 </param>
		void UpdateTlas(BlasVector arg_blasVector);

		/// <summary>
		/// アドレスに情報を書き込む処理
		/// </summary>
		/// <param name="m_mapAddress"> データの送り先 </param>
		/// <param name="m_pData"> 送るデータ </param>
		/// <param name="m_dataSize"> 送るデータのサイズ </param>
		void WriteToMemory(void* m_mapAddress, const void* m_pData, size_t m_dataSize);

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

	};

}