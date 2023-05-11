#include "DrawingByRasterize.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

//�e�X�g�p�A�p�C�v���C���̃n���h�����Ƀ\�[�g��������
int int_cmpr(const DrawingByRasterize::DrawData *a, const DrawingByRasterize::DrawData *b)
{
	RESOURCE_HANDLE lAHandle = a->pipelineHandle, lBHandle = b->pipelineHandle;

	if (lAHandle < lBHandle)
	{
		return 1;
	}
	else if (lBHandle < lAHandle)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
//�����_�[�^�[�Q�b�g,�[�x,��,�p�C�v���C���̗D��x�Ń\�[�g����\��


DrawingByRasterize::DrawingByRasterize()
{
}

void DrawingByRasterize::Update()
{
	//�K�v�ȏ��𐶐�
	for (int lGenerateIndex = 0; lGenerateIndex < graphicDataArray.size(); ++lGenerateIndex)
	{
		//�p�C�v���C���̏������ɐ����B�p�C�v���C���̃n���h����o�^���A�`�掞�ɂ��̃n���h���ŎQ�Ƃ���悤�ɂ���
		if (!graphicDataArray[lGenerateIndex].generateFlag)
		{
			//�V�F�[�_�[�̃R���p�C��
			for (int i = 0; i < graphicDataArray[lGenerateIndex].shaderDataArray.size(); ++i)
			{
				RESOURCE_HANDLE lShaderHandle = shaderBufferMgr.GenerateShader(graphicDataArray[lGenerateIndex].shaderDataArray[i]);
				ErrorCheck(lShaderHandle, graphicDataArray[lGenerateIndex].drawCallData);

				graphicDataArray[lGenerateIndex].shaderHandleArray.emplace_back(lShaderHandle);
				D3D12_SHADER_BYTECODE shaderByteCode = CD3DX12_SHADER_BYTECODE(shaderBufferMgr.GetBuffer(lShaderHandle)->GetBufferPointer(), shaderBufferMgr.GetBuffer(lShaderHandle)->GetBufferSize());
				switch (graphicDataArray[lGenerateIndex].shaderDataArray[i].shaderType)
				{
				case SHADER_TYPE_VERTEX:
					graphicDataArray[lGenerateIndex].pipelineData.VS = shaderByteCode;
					break;
				case SHADER_TYPE_PIXEL:
					graphicDataArray[lGenerateIndex].pipelineData.PS = shaderByteCode;
					break;
				case SHADER_TYPE_GEOMETORY:
					graphicDataArray[lGenerateIndex].pipelineData.GS = shaderByteCode;
					break;
				default:
					break;
				}
			}


			


			RootSignatureDataTest lRootSignatureGenerateData;
			//���[�g�V�O�l�`���̐���
			for (int i = 0; i < graphicDataArray[lGenerateIndex].buffer.size(); ++i)
			{
				if (lGenerateIndex == 2 && graphicDataArray[lGenerateIndex].buffer[i].rangeType == GRAPHICS_RANGE_TYPE_UAV_DESC)
				{
					lRootSignatureGenerateData.rangeArray.emplace_back
					(
						GRAPHICS_RANGE_TYPE_UAV_DESC,
						graphicDataArray[lGenerateIndex].buffer[i].rootParamType
					);
				}
				else
				{
					lRootSignatureGenerateData.rangeArray.emplace_back
					(
						graphicDataArray[lGenerateIndex].buffer[i].rangeType,
						graphicDataArray[lGenerateIndex].buffer[i].rootParamType
					);
				}
			}
			graphicDataArray[lGenerateIndex].rootsignatureHandle = rootSignatureBufferMgr.GenerateRootSignature(lRootSignatureGenerateData);


			//�p�C�v���C���̐���
			graphicDataArray[lGenerateIndex].pipelineData.pRootSignature = rootSignatureBufferMgr.GetBuffer(graphicDataArray[lGenerateIndex].rootsignatureHandle).Get();
			graphicDataArray[lGenerateIndex].pipelineHandle = piplineBufferMgr.GeneratePipeline(graphicDataArray[lGenerateIndex].pipelineData);
			ErrorCheck(graphicDataArray[lGenerateIndex].pipelineHandle, graphicDataArray[lGenerateIndex].drawCallData);


			graphicDataArray[lGenerateIndex].generateFlag = true;
		}
	}
}

void DrawingByRasterize::Draw()
{

	//���܂ŃX�^�b�N�����`��������Ƃɕ`��
	for (int lDrawIndex = 0; lDrawIndex < graphicDataArray.size(); ++lDrawIndex)
	{
		//�p�C�v���C���ƃ��[�g�V�O�l�`���̐���
		RESOURCE_HANDLE lPipelineHandle = graphicDataArray[lDrawIndex].pipelineHandle;
		RESOURCE_HANDLE lRootSignatureHandle = graphicDataArray[lDrawIndex].rootsignatureHandle;

		DirectX12CmdList::Instance()->cmdList->SetGraphicsRootSignature(
			rootSignatureBufferMgr.GetBuffer(lRootSignatureHandle).Get()
		);
		DirectX12CmdList::Instance()->cmdList->SetPipelineState(
			piplineBufferMgr.GetBuffer(lPipelineHandle).Get()
		);

		//���[�g�V�O�l�`���[�̏������Ƀo�b�t�@��ς�
		SetBufferOnCmdList(graphicDataArray[lDrawIndex].buffer, rootSignatureBufferMgr.GetRootParam(lRootSignatureHandle));

		//���_����ςށBIndexBufferView�̃o�C�g����0�̏ꍇ��DrawInstance���g�p���Ă���Ɛ�������
		if (graphicDataArray[lDrawIndex].drawIndexInstanceCommandData.indexBufferView.SizeInBytes != 0)
		{
			DrawIndexInstanceCommand(graphicDataArray[lDrawIndex].drawIndexInstanceCommandData);
		}
		else
		{
			DrawInstanceCommand(graphicDataArray[lDrawIndex].drawInstanceCommandData);
		}
	}
}

void DrawingByRasterize::Clear()
{
	graphicDataArray.clear();
	graphicDataArray.shrink_to_fit();
}

RESOURCE_HANDLE DrawingByRasterize::GetHandle()
{
	RESOURCE_HANDLE lHandle = drawHandle.GetHandle();
	if (graphicDataArray.size() <= lHandle)
	{
		graphicDataArray.emplace_back();
	}
	return lHandle;
}

DrawingByRasterize::DrawData *DrawingByRasterize::StackData(RESOURCE_HANDLE HANDLE)
{
	return &graphicDataArray[HANDLE];
}

void DrawingByRasterize::SetBufferOnCmdList(const std::vector<KazBufferHelper::BufferData> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM)
{
	for (int i = 0; i < BUFFER_ARRAY.size(); ++i)
	{
		const int L_PARAM = KazRenderHelper::SetBufferOnCmdList(ROOT_PARAM, BUFFER_ARRAY[i].rangeType, BUFFER_ARRAY[i].rootParamType);

		//�f�X�N���v�^�q�[�v�ɃR�}���h���X�g�ɐςށB�]�肪�����Ȃ�f�X�N���v�^�q�[�v���Ɣ��f����
		if (BUFFER_ARRAY[i].rangeType % 2 == 0)
		{
			DirectX12CmdList::Instance()->cmdList->SetGraphicsRootDescriptorTable(L_PARAM, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(BUFFER_ARRAY[i].GetViewHandle()));
			continue;
		}

		//�r���[�Őςލۂ͂��ꂼ��̎�ނɍ��킹�ăR�}���h���X�g�ɐς�
		switch (BUFFER_ARRAY[i].rangeType)
		{
		case GRAPHICS_RANGE_TYPE_SRV_VIEW:
			DirectX12CmdList::Instance()->cmdList->SetGraphicsRootShaderResourceView(L_PARAM, BUFFER_ARRAY[i].bufferWrapper->GetBuffer()->GetGPUVirtualAddress());
			break;
		case GRAPHICS_RANGE_TYPE_UAV_VIEW:
			DirectX12CmdList::Instance()->cmdList->SetGraphicsRootUnorderedAccessView(L_PARAM, BUFFER_ARRAY[i].bufferWrapper->GetBuffer()->GetGPUVirtualAddress());
			break;
		case GRAPHICS_RANGE_TYPE_CBV_VIEW:
			DirectX12CmdList::Instance()->cmdList->SetGraphicsRootConstantBufferView(L_PARAM, BUFFER_ARRAY[i].bufferWrapper->GetBuffer()->GetGPUVirtualAddress());
			break;
		default:
			break;
		}
	}
}

void DrawingByRasterize::DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData &DATA)
{
	//�`�施��-----------------------------------------------------------------------------------------------------
	DirectX12CmdList::Instance()->cmdList->IASetPrimitiveTopology(DATA.topology);
	DirectX12CmdList::Instance()->cmdList->IASetVertexBuffers(DATA.vertexBufferDrawData.slot, DATA.vertexBufferDrawData.numViews, &DATA.vertexBufferDrawData.vertexBufferView);
	DirectX12CmdList::Instance()->cmdList->IASetIndexBuffer(&DATA.indexBufferView);
	DirectX12CmdList::Instance()->cmdList->DrawIndexedInstanced(
		DATA.drawIndexInstancedData.indexCountPerInstance,
		DATA.drawIndexInstancedData.instanceCount,
		DATA.drawIndexInstancedData.startIndexLocation,
		DATA.drawIndexInstancedData.baseVertexLocation,
		DATA.drawIndexInstancedData.startInstanceLocation
	);
	//�`�施��-----------------------------------------------------------------------------------------------------
}

void DrawingByRasterize::DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData &DATA)
{
	DirectX12CmdList::Instance()->cmdList->IASetPrimitiveTopology(DATA.topology);
	DirectX12CmdList::Instance()->cmdList->IASetVertexBuffers(DATA.vertexBufferDrawData.slot, DATA.vertexBufferDrawData.numViews, &DATA.vertexBufferDrawData.vertexBufferView);
	DirectX12CmdList::Instance()->cmdList->DrawInstanced(
		DATA.drawInstanceData.vertexCountPerInstance,
		DATA.drawInstanceData.instanceCount,
		DATA.drawInstanceData.startVertexLocation,
		DATA.drawInstanceData.startInstanceLocation
	);
}

std::string DrawingByRasterize::ErrorMail(const std::source_location &DRAW_SOURCE_LOCATION)
{
	std::string lFunctionString = DRAW_SOURCE_LOCATION.function_name();
	std::string lFileNameString = DRAW_SOURCE_LOCATION.file_name();
	std::string lColumn = std::to_string(DRAW_SOURCE_LOCATION.column());
	std::string lLine = std::to_string(DRAW_SOURCE_LOCATION.line());

	return lFileNameString + "�t�@�C����" + lFunctionString + "�֐���" + lLine + "�s�ڂ�" + lColumn + "�����ڂɏ�����Ă���`��N���X�Ő������ꂽ���ɖ�肪����܂�";
}