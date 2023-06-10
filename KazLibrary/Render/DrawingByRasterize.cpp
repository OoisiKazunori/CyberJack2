#include "DrawingByRasterize.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"

//テスト用、パイプラインのハンドル順にソートをかける
int int_cmpr(const DrawFunc::DrawData *a, const DrawFunc::DrawData *b)
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


			if (graphicDataArray[lGenerateIndex].materialBuffer.size() != 0)
			{
				const int FIRST_MESH_INDEX = 0;
				//マテリアルバッファを見てルートシグネチャーの情報詰め込み
				//全メッシュ共通で入るマテリアル情報のスタックを見てルートシグネチャーの最初に積める
				for (int i = 0; i < MATERIAL_TEXTURE_MAX; ++i)
				{
					lRootSignatureGenerateData.rangeArray.emplace_back
					(
						graphicDataArray[lGenerateIndex].materialBuffer[FIRST_MESH_INDEX][i].rangeType,
						graphicDataArray[lGenerateIndex].materialBuffer[FIRST_MESH_INDEX][i].rootParamType
					);
				}
			}
			//その他バッファを見てルートシグネチャーの情報詰め込み
			for (int i = 0; i < graphicDataArray[lGenerateIndex].buffer.size(); ++i)
			{
				lRootSignatureGenerateData.rangeArray.emplace_back
				(
					graphicDataArray[lGenerateIndex].buffer[i].rangeType,
					graphicDataArray[lGenerateIndex].buffer[i].rootParamType
				);
			}
			graphicDataArray[lGenerateIndex].rootsignatureHandle = rootSignatureBufferMgr.GenerateRootSignature(lRootSignatureGenerateData);


			//パイプラインの生成
			graphicDataArray[lGenerateIndex].pipelineData.pRootSignature = rootSignatureBufferMgr.GetBuffer(graphicDataArray[lGenerateIndex].rootsignatureHandle).Get();
			graphicDataArray[lGenerateIndex].pipelineHandle = piplineBufferMgr.GeneratePipeline(graphicDataArray[lGenerateIndex].pipelineData);
			graphicDataArray[lGenerateIndex].pipelineHandle = -1;
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

		//描画コマンド実行
		switch (graphicDataArray[lDrawIndex].drawCommandType)
		{
		case DrawFunc::VERT_TYPE::INDEX:
			DrawIndexInstanceCommand(graphicDataArray[lDrawIndex].drawIndexInstanceCommandData);
			break;
		case DrawFunc::VERT_TYPE::INSTANCE:
			DrawInstanceCommand(graphicDataArray[lDrawIndex].drawInstanceCommandData);
			break;
		case DrawFunc::VERT_TYPE::MULTI_MESHED:
			MultiMeshedDrawIndexInstanceCommand(graphicDataArray[lDrawIndex].drawMultiMeshesIndexInstanceCommandData, graphicDataArray[lDrawIndex].materialBuffer, rootSignatureBufferMgr.GetRootParam(lRootSignatureHandle));
			break;
		default:
			break;
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

DrawFunc::DrawData *DrawingByRasterize::StackData(RESOURCE_HANDLE HANDLE)
{
	return &graphicDataArray[HANDLE];
}

void DrawingByRasterize::ObjectRender(const DrawFunc::DrawCallData &DRAW_DATA)
{
	kazCommandList.emplace_back(DRAW_DATA);
}

void DrawingByRasterize::Sort()
{
	renderInfomationForDirectX12Array.clear();

	kazCommandList.sort([](DrawFunc::DrawCallData a, DrawFunc::DrawCallData b)
		{
			RESOURCE_HANDLE lAHandle = a.renderTargetHandle, lBHandle = b.renderTargetHandle;
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
		});


	//ソートが終わったらDirectX12のコマンドリストに命令出来るように描画情報を生成する。
	for (auto &callData : kazCommandList)
	{
		DrawFunc::DrawData result;

		//シェーダーのコンパイル
		for (int i = 0; i < callData.pipelineData.shaderDataArray.size(); ++i)
		{
			RESOURCE_HANDLE lShaderHandle = shaderBufferMgr.GenerateShader(callData.pipelineData.shaderDataArray[i]);
			ErrorCheck(lShaderHandle, callData.callLocation);

			result.shaderHandleArray.emplace_back(lShaderHandle);
			D3D12_SHADER_BYTECODE shaderByteCode = CD3DX12_SHADER_BYTECODE(shaderBufferMgr.GetBuffer(lShaderHandle)->GetBufferPointer(), shaderBufferMgr.GetBuffer(lShaderHandle)->GetBufferSize());
			switch (result.shaderDataArray[i].shaderType)
			{
			case SHADER_TYPE_VERTEX:
				result.pipelineData.VS = shaderByteCode;
				break;
			case SHADER_TYPE_PIXEL:
				result.pipelineData.PS = shaderByteCode;
				break;
			case SHADER_TYPE_GEOMETORY:
				result.pipelineData.GS = shaderByteCode;
				break;
			default:
				break;
			}
		}


		RootSignatureDataTest lRootSignatureGenerateData;


		if (callData.materialBuffer.size() != 0)
		{
			const int FIRST_MESH_INDEX = 0;
			//マテリアルバッファを見てルートシグネチャーの情報詰め込み
			//全メッシュ共通で入るマテリアル情報のスタックを見てルートシグネチャーの最初に積める
			for (int i = 0; i < MATERIAL_TEXTURE_MAX; ++i)
			{
				lRootSignatureGenerateData.rangeArray.emplace_back
				(
					callData.materialBuffer[FIRST_MESH_INDEX][i].rangeType,
					callData.materialBuffer[FIRST_MESH_INDEX][i].rootParamType
				);
			}
		}
		//その他バッファを見てルートシグネチャーの情報詰め込み
		for (int i = 0; i < callData.bufferResourceDataArray.size(); ++i)
		{
			lRootSignatureGenerateData.rangeArray.emplace_back
			(
				callData.bufferResourceDataArray[i].rangeType,
				callData.bufferResourceDataArray[i].rootParam
			);
		}
		result.rootsignatureHandle = rootSignatureBufferMgr.GenerateRootSignature(lRootSignatureGenerateData);


		//パイプラインの生成
		result.pipelineData.pRootSignature = rootSignatureBufferMgr.GetBuffer(result.rootsignatureHandle).Get();
		result.pipelineHandle = piplineBufferMgr.GeneratePipeline(result.pipelineData);
		ErrorCheck(result.pipelineHandle, callData.callLocation);

		renderInfomationForDirectX12Array.emplace_back(result);
	}


	kazCommandList.clear();
}

void DrawingByRasterize::Render()
{
	for (auto &renderData : renderInfomationForDirectX12Array)
	{
		//パイプラインとルートシグネチャの生成
		RESOURCE_HANDLE lPipelineHandle = renderData.pipelineHandle;
		RESOURCE_HANDLE lRootSignatureHandle = renderData.rootsignatureHandle;

		DirectX12CmdList::Instance()->cmdList->SetGraphicsRootSignature(
			rootSignatureBufferMgr.GetBuffer(lRootSignatureHandle).Get()
		);
		DirectX12CmdList::Instance()->cmdList->SetPipelineState(
			piplineBufferMgr.GetBuffer(lPipelineHandle).Get()
		);
		//ルートシグネチャーの情報を元にバッファを積む
		SetBufferOnCmdList(renderData.buffer, rootSignatureBufferMgr.GetRootParam(lRootSignatureHandle));

		//描画コマンド実行
		switch (renderData.drawCommandType)
		{
		case DrawFunc::VERT_TYPE::INDEX:
			DrawIndexInstanceCommand(renderData.drawIndexInstanceCommandData);
			break;
		case DrawFunc::VERT_TYPE::INSTANCE:
			DrawInstanceCommand(renderData.drawInstanceCommandData);
			break;
		case DrawFunc::VERT_TYPE::MULTI_MESHED:
			MultiMeshedDrawIndexInstanceCommand(renderData.drawMultiMeshesIndexInstanceCommandData, renderData.materialBuffer, rootSignatureBufferMgr.GetRootParam(lRootSignatureHandle));
			break;
		default:
			break;
		}
	}
}

void DrawingByRasterize::SetBufferOnCmdList(const std::vector<KazBufferHelper::BufferData> &BUFFER_ARRAY, std::vector<RootSignatureParameter> ROOT_PARAM)
{
	for (int i = 0; i < BUFFER_ARRAY.size(); ++i)
	{
		const int L_PARAM = KazRenderHelper::SetBufferOnCmdList(ROOT_PARAM, BUFFER_ARRAY[i].rangeType, BUFFER_ARRAY[i].rootParamType);

		//デスクリプタヒープにコマンドリストに積む。余りが偶数ならデスクリプタヒープだと判断する
		if (BUFFER_ARRAY[i].rangeType % 2 == 0)
		{
			DirectX12CmdList::Instance()->cmdList->SetGraphicsRootDescriptorTable(L_PARAM, DescriptorHeapMgr::Instance()->GetGpuDescriptorView(BUFFER_ARRAY[i].bufferWrapper->GetViewHandle()));
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

void DrawingByRasterize::MultiMeshedDrawIndexInstanceCommand(const KazRenderHelper::MultipleMeshesDrawIndexInstanceCommandData &DATA, const std::vector<std::vector<KazBufferHelper::BufferData>> &MATERIAL_BUFFER, std::vector<RootSignatureParameter> ROOT_PARAM)
{
	//描画命令-----------------------------------------------------------------------------------------------------
	const int COMMAND_MAX_DATA = static_cast<int>(DATA.vertexBufferDrawData.size());

	for (int i = 0; i < COMMAND_MAX_DATA; ++i)
	{
		//マテリアル情報のスタック
		SetBufferOnCmdList(MATERIAL_BUFFER[i], ROOT_PARAM);

		DirectX12CmdList::Instance()->cmdList->IASetPrimitiveTopology(DATA.topology);
		DirectX12CmdList::Instance()->cmdList->IASetVertexBuffers(DATA.vertexBufferDrawData[i].slot, DATA.vertexBufferDrawData[i].numViews, &DATA.vertexBufferDrawData[i].vertexBufferView);
		DirectX12CmdList::Instance()->cmdList->IASetIndexBuffer(&DATA.indexBufferView[i]);
		DirectX12CmdList::Instance()->cmdList->DrawIndexedInstanced(
			DATA.drawIndexInstancedData[i].indexCountPerInstance,
			DATA.drawIndexInstancedData[i].instanceCount,
			DATA.drawIndexInstancedData[i].startIndexLocation,
			DATA.drawIndexInstancedData[i].baseVertexLocation,
			DATA.drawIndexInstancedData[i].startInstanceLocation
		);
	}
	//描画命令-----------------------------------------------------------------------------------------------------
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