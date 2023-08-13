#pragma once
#include <Helper/Compute.h>
#include "../DirectXCommon/DirectX12.h"

namespace PostEffect {

	class GaussianBlur;

	//実行することでGBufferからシーン情報と明るさ情報を持ってきてBloomをかけてくれるクラス。
	class Bloom {

	public:

		/*===== 変数 =====*/

		//入力されたテクスチャとそのコピーテクスチャ
		KazBufferHelper::BufferData m_targetTexture;
		KazBufferHelper::BufferData m_targetCopyTexture;

		//川瀬式ブルームをかける際に必要なテクスチャ
		static const int BLUR_TEXTURE_COUNT = 4;
		std::array<KazBufferHelper::BufferData, BLUR_TEXTURE_COUNT> m_blurTextures;

		//ブラーをかけてくれるやつ。
		std::array<std::shared_ptr<GaussianBlur>, BLUR_TEXTURE_COUNT + 1> m_blur;

		//最終合成用シェーダー
		ComputeShader m_composeShader;

		//その他
		KazMath::Vec2<UINT> TEXSIZE = KazMath::Vec2<UINT>(1280, 720);


	public:

		/*===== 関数 =====*/

		//コンストラクタ
		Bloom(KazBufferHelper::BufferData arg_bloomTargetTexture);

		//ブルームをかける。
		void Apply();

	private:

		//テクスチャをコピーする。
		void CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture);

		//バッファの状態を遷移させる。
		void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

	};

}