#include "DrawingByRasterize.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

//テスト用、パイプラインのハンドル順にソートをかける
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
//レンダーターゲット,深度,α,パイプラインの優先度でソートする予定


DrawingByRasterize::DrawingByRasterize()
{
}

void DrawingByRasterize::Update()
{
	//必要な情報を生成
	for (int lGenerateIndex = 0; lGenerateIndex < graphicDataArray.size(); ++lGenerateIndex)
	{
		//パイプラインの情報を元に生成。パイプラインのハンドルを登録し、描画時にそのハンドルで参照するようにする
		if (!graphicDataArray[lGenerateIndex].generateFlag)
		{
			//シェーダーのコンパイル
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
			//ルートシグネチャの生成
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


			//パイプラインの生成
			graphicDataArray[lGenerateIndex].pipelineData.pRootSignature = rootSignatureBufferMgr.GetBuffer(graphicDataArray[lGenerateIndex].rootsignatureHandle).Get();
			graphicDataArray[lGenerateIndex].pipelineHandle = piplineBufferMgr.GeneratePipeline(graphicDataArray[lGenerateIndex].pipelineData);
			ErrorCheck(graphicDataArray[lGenerateIndex].pipelineHandle, graphicDataArray[lGenerateIndex].drawCallData);


			graphicDataArray[lGenerateIndex].generateFlag = true;
		}
	}
}

void DrawingByRasterize::Draw()
{

	//今までスタックした描画情報をもとに描画
	for (int lDrawIndex = 0; lDrawIndex < graphicDataArray.size(); ++lDrawIndex)
	{
		//パイプラインとルートシグネチャの生成
		RESOURCE_HANDLE lPipelineHandle = graphicDataArray[lDrawIndex].pipelineHandle;
		RESOURCE_HANDLE lRootSignatureHandle = graphicDataArray[lDrawIndex].rootsignatureHandle;

		DirectX12CmdList::Instance()->cmdList->SetGraphicsRootSignature(
			rootSignatureBufferMgr.GetBuffer(lRootSignatureHandle).Get()
		);
		DirectX12CmdList::Instance()->cmdList->SetPipelineState(
			piplineBufferMgr.GetBuffer(lPipelineHandle).Get()
		);

		//ルートシグネチャーの情報を元にバッファを積む
		SetBufferOnCmdList(graphicDataArray[lDrawIndex].buffer, rootSignatureBufferMgr.GetRootParam(lRootSignatureHandle));

		//頂点情報を積む。IndexBufferViewのバイト数が0の場合はDrawInstanceを使用していると推測する
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

		//デスクリプタヒープにコマンドリストに積む。余りが偶数ならデスクリプタヒープだと判断する
		if (BUFFER_ARRAY[i].rangeType % 2 == 0)
		{
			DirectX12CmdList::Instance()->cmdList->SetGraphicsRootDescriptorTable(L_PARAM, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(BUFFER_ARRAY[i].GetViewHandle()));
			continue;
		}

		//ビューで積む際はそれぞれの種類に合わせてコマンドリストに積む
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
	//描画命令-----------------------------------------------------------------------------------------------------
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
	//描画命令-----------------------------------------------------------------------------------------------------
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

	return lFileNameString + "ファイルの" + lFunctionString + "関数の" + lLine + "行目の" + lColumn + "文字目に書かれている描画クラスで生成された情報に問題があります";
}