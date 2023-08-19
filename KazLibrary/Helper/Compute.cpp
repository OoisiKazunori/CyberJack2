#include "Compute.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

ComputeShader::ComputeShader() :m_initFlag(false)
{
}

void ComputeShader::Generate(const ShaderOptionData& arg_shader, std::vector<KazBufferHelper::BufferData> arg_extraBuffer)
{
	//シェーダーの生成
	RESOURCE_HANDLE shaderHandle = m_shaderBuffer.GenerateShader(arg_shader);
	//ルートシグネチャーの生成
	RootSignatureDataTest data = m_rootSignatureBuffer.GetGenerateData(arg_extraBuffer);
	RESOURCE_HANDLE m_rootsignatureHandle = m_rootSignatureBuffer.GenerateRootSignature(data);

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	//パイプラインの生成
	desc.CS = CD3DX12_SHADER_BYTECODE(m_shaderBuffer.GetBuffer(shaderHandle)->GetBufferPointer(), m_shaderBuffer.GetBuffer(shaderHandle)->GetBufferSize());
	desc.pRootSignature = m_rootSignatureBuffer.GetBuffer(m_rootsignatureHandle).Get();
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	RESOURCE_HANDLE pipelineHandle = m_piplineBuffer.GeneratePipeline(desc);
	if (pipelineHandle == -1)
	{
		assert(0);
	}
	m_initFlag = true;


	m_extraBufferArray = arg_extraBuffer;
}

void ComputeShader::Generate(const ShaderOptionData& arg_shader,const RootSignatureDataTest &arg_rootsignature)
{
	//シェーダーの生成
	RESOURCE_HANDLE shaderHandle = m_shaderBuffer.GenerateShader(arg_shader);
	//ルートシグネチャーの生成
	RootSignatureDataTest data = arg_rootsignature;
	RESOURCE_HANDLE m_rootsignatureHandle = m_rootSignatureBuffer.GenerateRootSignature(data);

	D3D12_COMPUTE_PIPELINE_STATE_DESC desc = {};
	//パイプラインの生成
	desc.CS = CD3DX12_SHADER_BYTECODE(m_shaderBuffer.GetBuffer(shaderHandle)->GetBufferPointer(), m_shaderBuffer.GetBuffer(shaderHandle)->GetBufferSize());
	desc.pRootSignature = m_rootSignatureBuffer.GetBuffer(m_rootsignatureHandle).Get();
	desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	desc.NodeMask = 0;
	RESOURCE_HANDLE pipelineHandle = m_piplineBuffer.GeneratePipeline(desc);
	if (pipelineHandle == -1)
	{
		assert(0);
	}
	m_initFlag = true;
}

void ComputeShader::Compute(const DispatchData& arg_dispatch)
{
	if (!m_initFlag)
	{
		assert(0);
	}

	DirectX12CmdList::Instance()->cmdList->SetPipelineState(
		m_piplineBuffer.GetBuffer(0).Get()
	);
	DirectX12CmdList::Instance()->cmdList->SetComputeRootSignature(
		m_rootSignatureBuffer.GetBuffer(0).Get()
	);
	SetBufferOnCmdList(m_extraBufferArray, m_rootSignatureBuffer.GetRootParam(0));
	DirectX12CmdList::Instance()->cmdList->Dispatch(arg_dispatch.x, arg_dispatch.y, arg_dispatch.z);
}

void ComputeShader::SetBufferOnCmdList(const std::vector<KazBufferHelper::BufferData>& BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM)
{
	for (int i = 0; i < BUFFER_ARRAY.size(); ++i)
	{
		const int L_PARAM = KazRenderHelper::SetBufferOnCmdList(ROOT_PARAM, BUFFER_ARRAY[i].rangeType, BUFFER_ARRAY[i].rootParamType);

		//デスクリプタヒープにコマンドリストに積む。余りが偶数ならデスクリプタヒープだと判断する
		if (BUFFER_ARRAY[i].rangeType % 2 == 0)
		{
			DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(L_PARAM, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(BUFFER_ARRAY[i].bufferWrapper->GetViewHandle()));
			continue;
		}

		//ビューで積む際はそれぞれの種類に合わせてコマンドリストに積む
		switch (BUFFER_ARRAY[i].rangeType)
		{
		case GRAPHICS_RANGE_TYPE_SRV_VIEW:
			DirectX12CmdList::Instance()->cmdList->SetComputeRootShaderResourceView(L_PARAM, BUFFER_ARRAY[i].bufferWrapper->GetBuffer()->GetGPUVirtualAddress());
			break;
		case GRAPHICS_RANGE_TYPE_UAV_VIEW:
			DirectX12CmdList::Instance()->cmdList->SetComputeRootUnorderedAccessView(L_PARAM, BUFFER_ARRAY[i].bufferWrapper->GetBuffer()->GetGPUVirtualAddress());
			break;
		case GRAPHICS_RANGE_TYPE_CBV_VIEW:
			DirectX12CmdList::Instance()->cmdList->SetComputeRootConstantBufferView(L_PARAM, BUFFER_ARRAY[i].bufferWrapper->GetBuffer()->GetGPUVirtualAddress());
			break;
		default:
			break;
		}
	}
}
