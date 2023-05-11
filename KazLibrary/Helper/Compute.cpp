#include "Compute.h"
#include"../DirectXCommon/DirectX12Device.h"
#include"../DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

void DispatchComputeShader::Stack(const ComputeData &STACK_DATA)
{
	computeArray.emplace_back(STACK_DATA);
}

void DispatchComputeShader::Update()
{
	for (auto &obj : computeArray)
	{
		if (!obj.isGenerateFlag)
		{
			//�V�F�[�_�[�̐���
			RESOURCE_HANDLE shaderHandle = shaderBufferMgr.GenerateShader(obj.shaderData);
			ErrorCheck(shaderHandle, obj.drawCallData);
			//���[�g�V�O�l�`���[�̐���
			RootSignatureDataTest data = rootSignatureBufferMgr.GetGenerateData(obj.bufferArray);
			RESOURCE_HANDLE rootsignatureHandle = rootSignatureBufferMgr.GenerateRootSignature(data);
			ErrorCheck(rootsignatureHandle, obj.drawCallData);

			//�p�C�v���C���̐���
			obj.desc.CS = CD3DX12_SHADER_BYTECODE(shaderBufferMgr.GetBuffer(shaderHandle)->GetBufferPointer(), shaderBufferMgr.GetBuffer(shaderHandle)->GetBufferSize());
			obj.desc.pRootSignature = rootSignatureBufferMgr.GetBuffer(rootsignatureHandle).Get();
			RESOURCE_HANDLE pipelineHandle = piplineBufferMgr.GeneratePipeline(obj.desc);
			ErrorCheck(pipelineHandle, obj.drawCallData);

			generateComputeArray.emplace_back(obj.dispatchData, obj.bufferArray, pipelineHandle, rootsignatureHandle, shaderHandle);
			obj.isGenerateFlag = true;
		}
	}
}

void DispatchComputeShader::Compute()
{
	//�R���s���[�g�V�F�[�_�[�̎��s
	for (auto &obj : generateComputeArray)
	{
		RESOURCE_HANDLE lPipelineHandle = obj.pipelineHandle;
		DirectX12CmdList::Instance()->cmdList->SetPipelineState(
			piplineBufferMgr.GetBuffer(lPipelineHandle).Get()
		);

		RESOURCE_HANDLE lRootSignatureHandle = obj.rootsignatureHandle;
		DirectX12CmdList::Instance()->cmdList->SetComputeRootSignature(
			rootSignatureBufferMgr.GetBuffer(lRootSignatureHandle).Get()
		);


		SetBufferOnCmdList(obj.bufferArray, rootSignatureBufferMgr.GetRootParam(lRootSignatureHandle));
		DirectX12CmdList::Instance()->cmdList->Dispatch(obj.dispatchData->x, obj.dispatchData->y, obj.dispatchData->z);
	}

}

void DispatchComputeShader::SetBufferOnCmdList(const std::vector<KazBufferHelper::BufferData> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM)

{
	for (int i = 0; i < BUFFER_ARRAY.size(); ++i)
	{
		const int L_PARAM = KazRenderHelper::SetBufferOnCmdList(ROOT_PARAM, BUFFER_ARRAY[i].rangeType, BUFFER_ARRAY[i].rootParamType);

		//�f�X�N���v�^�q�[�v�ɃR�}���h���X�g�ɐςށB�]�肪�����Ȃ�f�X�N���v�^�q�[�v���Ɣ��f����
		if (BUFFER_ARRAY[i].rangeType % 2 == 0)
		{
			DirectX12CmdList::Instance()->cmdList->SetComputeRootDescriptorTable(L_PARAM, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(BUFFER_ARRAY[i].GetViewHandle()));
			continue;
		}

		//�r���[�Őςލۂ͂��ꂼ��̎�ނɍ��킹�ăR�}���h���X�g�ɐς�
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
