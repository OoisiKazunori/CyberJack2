#pragma once
#include <Helper/Compute.h>
#include "../DirectXCommon/DirectX12.h"

namespace PostEffect {

	class GaussianBlur;
	class Bloom;

	//実行することでGBufferからシーン情報と明るさ情報を持ってきてレンズフレアをかけてくれるクラス。
	class LensFlare {

	public:			//後でPrivateにしろ！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

		/*===== 変数 =====*/

		//レンズフレアパス関連
		ComputeShader m_lensFlareShader;					//レンズフレアをかけるシェーダー
		KazBufferHelper::BufferData m_lensFlareTexture;		//レンズフレアをかけたテクスチャ
		KazBufferHelper::BufferData m_lensColorTexture;		//レンズの色テクスチャ
		KazMath::Vec2<UINT> LENSFLARE_TEXSIZE = KazMath::Vec2<UINT>(1280, 720);

		//ブルーム(フレア)パス
		KazBufferHelper::BufferData m_bloomTexture;
		std::shared_ptr<Bloom> m_bloom;

		//最終加工パス関連
		ComputeShader m_finalProcessingShader;				//最終加工 and 合成用シェーダー
		KazBufferHelper::BufferData m_lendDirtTexture;		//レンズの汚れテクスチャ
		KazBufferHelper::BufferData m_lensStarTexture;		//レンズのスターバーストテクスチャ
		KazBufferHelper::BufferData m_cametaVecConstBuffer;	//レンズのスターバーストを回転させる定数バッファ
		struct CameraVec {
			KazMath::Vec3<float> m_cameraXVec;
			float m_pad;
			KazMath::Vec3<float> m_cameraZVec;
			float m_pad2;
		}m_cameraVec;
		KazMath::Vec2<UINT> BACKBUFFER_SIZE = KazMath::Vec2<UINT>(1280, 720);

		//ブラーパス関連
		std::shared_ptr<GaussianBlur> m_blurPath;

		//その他
		KazBufferHelper::BufferData m_lensFlareTargetTexture;	//コンストラクタで設定される、レンズフレアをかける対象。
		DirectX12* m_refDirectX12;
		KazBufferHelper::BufferData m_lensFlareTargetCopyTexture;	//レンズフレアをかける対象をコピーしたテクスチャ
		KazMath::Vec2<UINT> COPY_TEXSIZE = KazMath::Vec2<UINT>(1280, 720);


	public:

		/*===== 関数 =====*/

		//コンストラクタ
		LensFlare(KazBufferHelper::BufferData arg_lnesflareTargetTexture, KazBufferHelper::BufferData arg_emissiveTexture);

		//レンズフレアをかける。
		void Apply();

	private:

		//レンズフレアをかけるテクスチャのコピーを作成する。
		void GenerateCopyOfLensFlareTexture();

		//バッファの状態を遷移させる。
		void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

		//テクスチャをコピーする。
		void CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture);

	};

}