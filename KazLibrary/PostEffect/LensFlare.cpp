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

		//各ブラーの出力結果用のテクスチャを生成。
		m_lensFlareTargetCopyTexture = KazBufferHelper::SetUAVTexBuffer(COPY_TEXSIZE.x, COPY_TEXSIZE.y, DXGI_FORMAT_R8G8B8A8_UNORM);
		m_lensFlareTargetCopyTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
		DescriptorHeapMgr::Instance()->CreateBufferView(
			m_lensFlareTargetCopyTexture.bufferWrapper->GetViewHandle(),
			KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), COPY_TEXSIZE.x * COPY_TEXSIZE.y),
			m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get()
		);
		{
			//レンズフレア用のシェーダーを用意。
			std::vector<KazBufferHelper::BufferData>extraBuffer =
			{
				 m_lensFlareTargetCopyTexture,
				 m_lensFlareTexture,
			};
			extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
			extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
			extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
			m_lensFlareShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/LensFlare/" + "LensFlareShader.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
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
		//m_lensFlareShader.Compute(lensFlareData);


		/*- ③ブラーパス -*/


		/*- ④最終加工パス -*/


	}

	void LensFlare::GenerateCopyOfLensFlareTexture()
	{

		/*===== レンズフレアテクスチャのコピーを作成 =====*/

		//レンズフレアをかける対象のステータスを変更。
		D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(
			m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			D3D12_RESOURCE_STATE_COPY_SOURCE),
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barriers), barriers);

		//一旦コピーしておく用のテクスチャのステータスを変更。
		D3D12_RESOURCE_BARRIER barrierToUAV[] = { CD3DX12_RESOURCE_BARRIER::Transition(
		m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(),
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
		D3D12_RESOURCE_STATE_COPY_DEST)
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, barrierToUAV);

		//コピーを実行
		DirectX12CmdList::Instance()->cmdList->CopyResource(m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(), m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get());

		//レンズフレアをかける対象のステータスを元に戻す。
		D3D12_RESOURCE_BARRIER barrierToCopy[] = { CD3DX12_RESOURCE_BARRIER::Transition(
			m_lensFlareTargetTexture.bufferWrapper->GetBuffer().Get(),
		D3D12_RESOURCE_STATE_COPY_SOURCE,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(1, barrierToCopy);

		//コピー先のステータスを元に戻す。
		D3D12_RESOURCE_BARRIER barrierToRenderTarget[] = {
		CD3DX12_RESOURCE_BARRIER::Transition(
		m_lensFlareTargetCopyTexture.bufferWrapper->GetBuffer().Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_UNORDERED_ACCESS)

		};
		DirectX12CmdList::Instance()->cmdList->ResourceBarrier(_countof(barrierToRenderTarget), barrierToRenderTarget);

	}

}