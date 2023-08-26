#pragma once
#include <Helper/Compute.h>
#include "Helper/KazBufferHelper.h"
#include "Math/KazMath.h"
#include <array>

//アウトラインを出力するクラス。
namespace PostEffect {

	class Outline {

	private:

		/*===== 変数 =====*/

		ComputeShader m_outlineShader;		//アウトラインを計算。

		KazBufferHelper::BufferData m_outlineTargetTexture;		//アウトラインをかける対象のテクスチャ
		KazBufferHelper::BufferData m_outputAlbedoTexture;		//アウトラインの色テクスチャ
		KazBufferHelper::BufferData m_outputEmissiveTexture;	//アウトラインのエミッシブのテクスチャ

		KazBufferHelper::BufferData m_outlineColorConstBuffer;	//アウトラインの色
		KazMath::Vec4<float> m_outlineColor;

	public:

		/*===== 関数 =====*/

		Outline(KazBufferHelper::BufferData arg_outlineTargetTexture);

		//アウトラインを書き込む
		void Apply();

		KazBufferHelper::BufferData GetOutputAlbedoTexture() { return m_outputAlbedoTexture; }
		KazBufferHelper::BufferData GetOutputEmissiveTexture() { return m_outputEmissiveTexture; }

	};

}