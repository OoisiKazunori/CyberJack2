#include "LensFlare.h"
#include "Buffer/GBufferMgr.h"
#include "Buffer/DescriptorHeapMgr.h"
#include "Buffer/UavViewHandleMgr.h"

namespace PostEffect {

	LensFlare::LensFlare(KazBufferHelper::BufferData arg_lnesflareTargetTexture, DirectX12* arg_refDirectX12)
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
		//レンズの色テクスチャをロード
		m_lensColorTexture = TextureResourceMgr::Instance()->LoadGraphBuffer(KazFilePathName::LensFlarePath + "lensColor.png");
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
				 m_lensFlareTexture,
				 m_lensFlareTargetTexture,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			m_finalProcessingShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "FinalProcessingShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
		}

	}

	void LensFlare::Apply() {

		/*===== レンズフレアをかける =====*/

		/*- ①レンズフレアをかける対象を一旦コピーしておく。 -*/

		GenerateCopyOfLensFlareTexture();


		/*- ②レンズフレアパス -*/

		//レンズフレアをかける。
		DispatchData lensFlareData;
		lensFlareData.x = static_cast<UINT>(LENSFLARE_TEXSIZE.x / 16) + 1;
		lensFlareData.y = static_cast<UINT>(LENSFLARE_TEXSIZE.y / 16) + 1;
		lensFlareData.z = static_cast<UINT>(1);
		m_lensFlareShader.Compute(lensFlareData);


		/*- ③ブラーパス -*/


		/*- ④最終加工パス -*/

		//最終加工 and 合成を行う。
		DispatchData finalPath;
		finalPath.x = static_cast<UINT>(BACKBUFFER_SIZE.x / 16) + 1;
		finalPath.y = static_cast<UINT>(BACKBUFFER_SIZE.y / 16) + 1;
		finalPath.z = static_cast<UINT>(1);
		m_finalProcessingShader.Compute(finalPath);


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

}