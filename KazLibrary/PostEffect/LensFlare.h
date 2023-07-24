#pragma once
#include <Helper/Compute.h>
#include "../DirectXCommon/DirectX12.h"

namespace PostEffect {

	//実行することでGBufferからシーン情報と明るさ情報を持ってきてレンズフレアをかけてくれるクラス。
	class LensFlare {

	public:			//後でPrivateにしろ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

		/*===== 変数 =====*/

		//ダウンサンプリングパス関連
		KazBufferHelper::BufferData m_lensFlareTargetCopyTexture;	//レンズフレアをかける対象をコピーしたテクスチャ
		KazMath::Vec2<UINT> COPY_TEXSIZE = KazMath::Vec2<UINT>(1280, 720);

		//レンズフレアパス関連
		ComputeShader m_lensFlareShader;					//レンズフレアをかけるシェーダー
		KazBufferHelper::BufferData m_lensFlareTexture;		//レンズフレアをかけたテクスチャ
		KazMath::Vec2<UINT> LENSFLARE_TEXSIZE = KazMath::Vec2<UINT>(1280, 720);

		//その他
		KazBufferHelper::BufferData m_lensFlareTargetTexture;	//コンストラクタで設定される、レンズフレアをかける対象。
		DirectX12* m_refDirectX12;


	public:

		/*===== 関数 =====*/

		//コンストラクタ
		LensFlare(KazBufferHelper::BufferData arg_lnesflareTargetTexture, DirectX12* arg_refDirectX12);

		//レンズフレアをかける。
		void Apply();

	private:

		//レンズフレアをかけるテクスチャのコピーを作成する。
		void GenerateCopyOfLensFlareTexture();

	};

}