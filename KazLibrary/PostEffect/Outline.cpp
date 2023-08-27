#include "Outline.h"
#include "Buffer/DescriptorHeapMgr.h"
#include "Buffer/UavViewHandleMgr.h"
#include "../PostEffect/GaussianBlur.h"

PostEffect::Outline::Outline(KazBufferHelper::BufferData arg_outlineTargetTexture)
{

	//アウトラインをかける対象のテクスチャを保存しておく。
	m_outlineTargetTexture = arg_outlineTargetTexture;

	//アウトラインの色
	m_outputAlbedoTexture = KazBufferHelper::SetUAVTexBuffer(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_outputAlbedoTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_outputAlbedoTexture.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), 1280 * 720),
		m_outputAlbedoTexture.bufferWrapper->GetBuffer().Get()
	);

	//アウトラインのエミッシブ
	m_outputEmissiveTexture = KazBufferHelper::SetUAVTexBuffer(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_outputEmissiveTexture.bufferWrapper->CreateViewHandle(UavViewHandleMgr::Instance()->GetHandle());
	DescriptorHeapMgr::Instance()->CreateBufferView(
		m_outputEmissiveTexture.bufferWrapper->GetViewHandle(),
		KazBufferHelper::SetUnorderedAccessTextureView(sizeof(DirectX::XMFLOAT4), 1280 * 720),
		m_outputEmissiveTexture.bufferWrapper->GetBuffer().Get()
	);

	//アウトラインの色
	m_outlineColor = KazMath::Vec4<float>(0.84f, 0.93f, 0.95f, 1);
	m_outlineColorConstBuffer = KazBufferHelper::SetConstBufferData(sizeof(KazMath::Vec4<float>));
	m_outlineColorConstBuffer.bufferWrapper->TransData(&m_outlineColor, sizeof(KazMath::Vec4<float>));

	//アウトライン計算用のシェーダー
	{

		std::vector<KazBufferHelper::BufferData>extraBuffer =
		{
			 m_outlineTargetTexture,
			 m_outputAlbedoTexture,
			 m_outputEmissiveTexture,
			 m_outlineColorConstBuffer,
		};
		extraBuffer[0].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[0].rootParamType = GRAPHICS_PRAMTYPE_TEX;
		extraBuffer[1].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[1].rootParamType = GRAPHICS_PRAMTYPE_TEX2;
		extraBuffer[2].rangeType = GRAPHICS_RANGE_TYPE_UAV_DESC;
		extraBuffer[2].rootParamType = GRAPHICS_PRAMTYPE_TEX3;
		extraBuffer[3].rangeType = GRAPHICS_RANGE_TYPE_CBV_VIEW;
		extraBuffer[3].rootParamType = GRAPHICS_PRAMTYPE_DATA;
		m_outlineShader.Generate(ShaderOptionData(KazFilePathName::RelativeShaderPath + "PostEffect/Outline/" + "Outline.hlsl", "main", "cs_6_4", SHADER_TYPE_COMPUTE), extraBuffer);
	}

}

void PostEffect::Outline::Apply()
{
	std::vector<D3D12_RESOURCE_BARRIER> barrier;

	barrier.emplace_back(CD3DX12_RESOURCE_BARRIER::UAV(m_outlineTargetTexture.bufferWrapper->GetBuffer().Get()));
	barrier.emplace_back(CD3DX12_RESOURCE_BARRIER::UAV(m_outputAlbedoTexture.bufferWrapper->GetBuffer().Get()));
	barrier.emplace_back(CD3DX12_RESOURCE_BARRIER::UAV(m_outputEmissiveTexture.bufferWrapper->GetBuffer().Get()));

	DirectX12CmdList::Instance()->cmdList->ResourceBarrier(static_cast<UINT>(barrier.size()), barrier.data());

	m_outlineColorConstBuffer.bufferWrapper->TransData(&m_outlineColor, sizeof(KazMath::Vec4<float>));
	DispatchData dispatchData;
	dispatchData.x = static_cast<UINT>(1280 / 16) + 1;
	dispatchData.y = static_cast<UINT>(720 / 16) + 1;
	dispatchData.z = static_cast<UINT>(1);
	m_outlineShader.Compute(dispatchData);

}
