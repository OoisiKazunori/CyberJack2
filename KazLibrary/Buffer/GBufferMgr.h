#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"
#include"../KazLibrary/Math/KazMath.h"
#include<memory>

namespace PostEffect {
	class GaussianBlur;
	class Outline;
}
class ComputeShader;

/// <summary>
/// G-Bufferの管理クラス
/// </summary>
class GBufferMgr :public ISingleton<GBufferMgr>
{
public:
	enum BufferType
	{
		NONE = -1,
		ALBEDO,
		NORMAL,
		R_M_S_ID,
		WORLD,
		EMISSIVE,
		MAX
	};
	GBufferMgr();

	std::vector<RESOURCE_HANDLE> GetRenderTarget();
	std::vector<DXGI_FORMAT> GetRenderTargetFormat();
	void SetCameraPos(DirectX::XMFLOAT3 arg_pos, DirectX::XMMATRIX arg_viewMat, DirectX::XMMATRIX arg_projMat)
	{
		m_cameraEyePosData.m_eyePos = KazMath::Vec3<float>(arg_pos.x, arg_pos.y, arg_pos.z);
		m_cameraEyePosData.m_viewMat = DirectX::XMMatrixInverse(nullptr, arg_viewMat);
		m_cameraEyePosData.m_projMat = DirectX::XMMatrixInverse(nullptr, arg_projMat);;
		m_cameraPosBuffer.bufferWrapper->TransData(&m_cameraEyePosData,sizeof(CameraEyePosBufferData));
	};
	const KazBufferHelper::BufferData &GetEyePosBuffer()
	{
		return m_cameraPosBuffer;
	}


	/// <summary>
	/// デバック用の描画確認
	/// </summary>
	void DebugDraw();

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(BufferType arg_type);

	const KazBufferHelper::BufferData &GetFinalBuffer()
	{
		return m_finalGBuffer;
	};

	const KazBufferHelper::BufferData &GetRayTracingBuffer()
	{
		return m_raytracingGBuffer;
	};

	const KazBufferHelper::BufferData& GetLensFlareBuffer()
	{
		return m_lensFlareLuminanceGBuffer;
	};

	const KazBufferHelper::BufferData& GetEmissiveGBuffer()
	{
		return m_emissiveGBuffer;
	};

	const KazBufferHelper::BufferData& GetBackBufferCopyBuffer()
	{
		return m_backBufferCopyBuffer;
	};

	const KazBufferHelper::BufferData& GetBackBufferCompositeBuffer()
	{
		return m_backBufferCompositeBuffer;
	};

	//バックバッファを合成
	void ComposeBackBuffer();

	//レンズフレア用のGBufferにブラーをかける。
	void ApplyLensFlareBlur();

	//レンズフレアとシーン画像を合成する。
	void ComposeLensFlareAndScene();

	//バッファのステータスを遷移。
	void BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after);

	//ライト用構造体
	struct DirLight {
		KazMath::Vec3<float> m_dir;
		int m_isActive;
	};
	struct PointLight {
		KazMath::Vec3<float> m_pos;
		float m_power;		//ライトが影響を与える最大距離
		KazMath::Vec3<float> m_pad;	//パラメーターが必要になったら適当に名前つけて変数を追加してください！
		int m_isActive;
	};
	struct LightConstData {
		DirLight m_dirLight;
		PointLight m_pointLight;
	}m_lightConstData;

	KazBufferHelper::BufferData m_lightBuffer;

	struct CameraEyePosBufferData {
		DirectX::XMMATRIX m_viewMat;
		DirectX::XMMATRIX m_projMat;
		KazMath::Vec3<float> m_eyePos;
		float m_noiseTimer;
	}m_cameraEyePosData;

	//レンズフレア用ブラー
	std::shared_ptr<PostEffect::GaussianBlur> m_lensFlareBlur;
	std::shared_ptr<PostEffect::Outline> m_outline;

private:
	//G-Buffer用のレンダーターゲット
	std::vector<RESOURCE_HANDLE>m_gBufferRenderTargetHandleArray;
	std::vector<DXGI_FORMAT>m_gBufferFormatArray;

	KazBufferHelper::BufferData m_cameraPosBuffer;

	//最終合成結果
	KazBufferHelper::BufferData m_finalGBuffer;
	KazBufferHelper::BufferData m_raytracingGBuffer;			//レイトレの出力結果
	KazBufferHelper::BufferData m_backBufferCopyBuffer;			//レイトレ結果をCopyする際に上書きしてしまわないようにするためのバッファ
	KazBufferHelper::BufferData m_backBufferCompositeBuffer;				//レイトレ結果をCopyする際に上書きしてしまわないようにするためのバッファ
	std::shared_ptr<ComputeShader> m_backBufferRaytracingCompositeShader;	//バックバッファとレイトレ出力結果を合成するバッファ

	//レンズフレア関連
	KazBufferHelper::BufferData m_lensFlareLuminanceGBuffer;	//レンズフレアに使用するGBuffer レイトレを実行すると書き込まれる。
	KazBufferHelper::BufferData m_lensFlareConposeBuffTexture;	//レンズフレアを合成するときに一旦保存するテクスチャ。
	KazBufferHelper::BufferData m_emissiveGBuffer;				//レンズフレアで使用するブルーム用GBuffer レイトレを実行すると書き込まれる。
	std::shared_ptr<ComputeShader> m_lensFlareComposeShader;
};

