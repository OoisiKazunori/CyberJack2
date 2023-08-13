#pragma once
#include <Helper/Compute.h>
#include "Helper/KazBufferHelper.h"
#include "Math/KazMath.h"
#include <array>

//ガウシアンブラーをかけるクラス
namespace PostEffect {

	class GaussianBlur {

	private:

		/*===== 変数 =====*/

		ComputeShader m_blurXShader;		//X方向のブラーのシェーダー
		ComputeShader m_blurYShader;		//Y方向のブラーのシェーダー
		ComputeShader m_composeShader;		//ブラーを合成するシェーダー

		KazBufferHelper::BufferData m_blurXResultTexture;	//X方向のブラーの出力画像
		KazBufferHelper::BufferData m_blurYResultTexture;	//Y方向のブラーの出力画像
		KazBufferHelper::BufferData m_blurTargetTexture;	//コンストラクタで設定される、ブラーをかける対象。

		KazBufferHelper::BufferData m_blurPowerConstBuffer;	//ガウシアンブラーの強さ

		static const int BLOOM_GAUSSIAN_WEIGHTS_COUNT = 8;	//ガウシアンブラーの強さに関する定数バッファのパラメーターの数。
		std::array<float, BLOOM_GAUSSIAN_WEIGHTS_COUNT> m_blurWeight;

		//ガウシアンブラーの強さ
		const float BLUR_POWER = 1000000.0f;

		//ブラーの出力先テクスチャのサイズ
		const KazMath::Vec2<UINT> BLURX_TEXSIZE = KazMath::Vec2<UINT>(1280 / 2, 720);
		const KazMath::Vec2<UINT> BLURY_TEXSIZE = KazMath::Vec2<UINT>(1280 / 2, 720 / 2);

	public:

		/*===== 関数 =====*/

		GaussianBlur(KazBufferHelper::BufferData arg_blurTargetTexture);

		//ブラーを実行
		void ApplyBlur();

	private:

		//ブラーの重みを計算。
		void CalcBloomWeightsTableFromGaussian();

	};

}