#include "LensFlare.h"
#include "Buffer/GBufferMgr.h"
#include "Buffer/DescriptorHeapMgr.h"
#include "Buffer/UavViewHandleMgr.h"
#include "../PostEffect/GaussianBlur.h"
#include "../PostEffect/Bloom.h"

namespace PostEffect {

	LensFlare::LensFlare(KazBufferHelper::BufferData arg_lnesflareTargetTexture, KazBufferHelper::BufferData arg_emissiveTexture)
	{

		/*===== コンストラクタ =====*/

		//レンズフレアをかける対象のテクスチャを保存しておく。
		m_lensFlareTargetTexture = arg_lnesflareTargetTexture;

		//レンズフレア対象の一旦保存用テクスチャを用意。
		m_lensFlareTargetCopyTexture = KazBufferHelper::SetUAVTexBuffer(COPY_TEXSIZE.x, COPY_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_lensFlareTargetCopyTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_lensFlareTargetCopyTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), COPY_TEXSIZE.x * COPY_TEXSIZE.y),
			m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get()
		);
		//レンズフレアの結果出力用のテクスチャを用意。
		m_lensFlareTexture = KazBufferHelper::SetUAVTexBuffer(LENSFLARE_TEXSIZE.x, LENSFLARE_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_lensFlareTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_lensFlareTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), LENSFLARE_TEXSIZE.x * LENSFLARE_TEXSIZE.y),
			m_lensFlareTexture.bufferWrapper->GetBuffer().Get()
		);
		//ブルーム用のテクスチャを用意。
		m_bloomTexture = arg_emissiveTexture;
		m_bloom = std::make_shared<PostEffect::Bloom>(m_bloomTexture);
		//レンズの色テクスチャをロード
		m_lensColorTexture = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensColor.png");
		m_lendDirtTexture = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensDirt.png");
		m_lensStarTexture = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensStar.png");
		//スターバースト回転用のバッファを用意。
		m_cametaVecConstBuffer = KazBufferHelper::SetConstBufferData(sizeof(CameraVec));
		{
			//レンズフレア用のシェーダーを用意。
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_lensColorTexture,
				 m_lensFlareTargetCopyTexture,
				 m_lensFlareTexture,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
			m_lensFlareShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "LensFlareShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}
		{
			//最終加工 and 合成パスを用意。
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_lendDirtTexture,
				 m_lensStarTexture,
				 m_lensFlareTexture,
				 m_bloomTexture,
				 m_lensFlareTargetTexture,
				 m_cametaVecConstBuffer,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_SRV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
			extraBuffer[3].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[3].rootParamType = GRAPHICS_PRAMTYPE_TEX4;
			extraBuffer[4].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[4].rootParamType = GRAPHICS_PRAMTYPE_TEX5;
			extraBuffer[5].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
			extraBuffer[5].rootParamType = GRAPHICS_PRAMTYPE_DATA;
			m_finalProcessingShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "FinalProcessingShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}

		//ブラーパス
		m_blurPath = std::make_shared<GaussianBlur>(m_lensFlareTexture);

	}

	void LensFlare::Apply() {

		/*===== レンズフレアをかける =====*/


		/*- ①レンズフレアをかける対象を一旦コピーしておく。 -*/

		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "LensFlare Path");
		GenerateCopyOfLensFlareTexture();


		/*- ②レンズフレアパス -*/

		//レンズフレアをかける。
		DispatchData lensFlareData;
		lensFlareData.x = static_cast<UINT>(LENSFLARE_TEXSIZE.x / 16) + 1;
		lensFlareData.y = static_cast<UINT>(LENSFLARE_TEXSIZE.y / 16) + 1;
		lensFlareData.z = static_cast<UINT>(1);
		m_lensFlareShader.Compute(lensFlareData);
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


		//ブルームもかけちゃう。
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Bloom");
		//CopyTexture(m_bloomTexture, m_lensFlareTargetCopyTexture);
		m_bloom->Apply();
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


		/*- ③ブラーパス -*/

		//レンズフレアにブラーをかける。
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Blur Path");
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		m_blurPath->ApplyBlur();
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());

		/*- ④最終加工パス -*/

		//カメラの情報を保存して転送。
		PIXBeginEvent(DirectX12CmdList::Instance()->cmdList.Get(), 0, "Final Path");
		m_cameraVec.m_cameraZVec = CameraMgr::Instance()->GetCameraAxis().z;
		m_cameraVec.m_cameraXVec = CameraMgr::Instance()->GetCameraAxis().x;
		KazMath::Vec3<float> yVec = m_cameraVec.m_cameraZVec.Cross(m_cameraVec.m_cameraXVec);
		m_cametaVecConstBuffer.bufferWrapper->TransData(&m_cameraVec, sizeof(CameraVec));

		//最終加工 and 合成を行う。
		DispatchData finalPath;
		finalPath.x = static_cast<UINT>(BACKBUFFER_SIZE.x / 16) + 1;
		finalPath.y = static_cast<UINT>(BACKBUFFER_SIZE.y / 16) + 1;
		finalPath.z = static_cast<UINT>(1);
		m_finalProcessingShader.Compute(finalPath);
		PIXEndEvent(DirectX12CmdList::Instance()->cmdList.Get());


	}

	void LensFlare::GenerateCopyOfLensFlareTexture()
	{

		/*===== レンズフレアテクスチャのコピーを作成 =====*/

		//レンズフレアをかける対象のステータスを変更。
		BufferStatesTransition(m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		//一旦コピーしておく用のテクスチャのステータスを変更。
		BufferStatesTransition(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		//コピーを実行
		DirectX12CmdList::Instance()->cmdList->CopyResource(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get());

		//レンズフレアをかける対象のステータスを元に戻す。
		BufferStatesTransition(m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		//コピー先のステータスを元に戻す。
		BufferStatesTransition(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

	void LensFlare::BufferStatesTransition(ID3D12Resource* arg_resource, D3D12_RESOURCE_STATES arg_before, D3D12_RESOURCE_STATES arg_after)
	{
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			arg_resource,
			arg_before,
			arg_after),
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);
	}

	void LensFlare::CopyTexture(KazBufferHelper::BufferData& arg_destTexture, KazBufferHelper::BufferData& arg_srcTexture)
	{

		/*===== レンズフレアテクスチャのコピーを作成 =====*/

		//レンズフレアをかける対象のステータスを変更。
		BufferStatesTransition(arg_srcTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);

		//一旦コピーしておく用のテクスチャのステータスを変更。
		BufferStatesTransition(arg_destTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST);

		//コピーを実行
		DirectX12CmdList::Instance()->cmdList->CopyResource(arg_destTexture.bufferWrapper->GetBuffer().Get(), arg_srcTexture.bufferWrapper->GetBuffer().Get());

		//レンズフレアをかける対象のステータスを元に戻す。
		BufferStatesTransition(arg_srcTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		//コピー先のステータスを元に戻す。
		BufferStatesTransition(arg_destTexture.bufferWrapper->GetBuffer().Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	}

}