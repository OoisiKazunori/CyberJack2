#include"../Render/IRender.h"

IRender::IRender()
{
	for (int i = 0; i < BUFFER_DATA_MAX; ++i)
	{
		constBufferDataName[i] = "";
		constBufferDataSize[i] = 0;
		constBufferRootParam[i] = GRAPHICS_PRAMTYPE_NONE;
		constBufferRangeType[i] = GRAPHICS_RANGE_TYPE_NONE;
		constBufferHandles[i] = -1;
	}
}

RESOURCE_HANDLE IRender::CreateConstBuffer(const unsigned int &TRANSMISSION_DATA, const string &ID, const GraphicsRangeType &RANGE, const GraphicsRootParamType &ROOTPARAM)
{
	RESOURCE_HANDLE lBufferHandle = 0;

	switch (RANGE)
	{
	case GRAPHICS_RANGE_TYPE_CBV_VIEW:
		lBufferHandle = gpuBuffer->CreateBuffer
		(
			KazBufferHelper::SetConstBufferData(TRANSMISSION_DATA)
		);
		break;
	case GRAPHICS_RANGE_TYPE_UAV_VIEW:
		lBufferHandle = gpuBuffer->CreateBuffer
		(
			KazBufferHelper::SetRWStructuredBuffer(TRANSMISSION_DATA)
		);
		break;
	}
	RESOURCE_HANDLE lGetHandle = handle.GetHandle();
	constBufferDataName[lGetHandle] = ID;
	constBufferDataSize[lGetHandle] = TRANSMISSION_DATA;
	constBufferRootParam[lGetHandle] = ROOTPARAM;
	constBufferRangeType[lGetHandle] = RANGE;
	constBufferHandles[lGetHandle] = lBufferHandle;

	return lBufferHandle;
}

void IRender::TransData(void *DATA, RESOURCE_HANDLE HANDLE, const string &ID)
{
	bool succeedFlag = false;
	for (int i = 0; i < constBufferDataName.size(); i++)
	{
		if (constBufferDataName[i] == ID)
		{
			gpuBuffer->TransData(HANDLE, DATA, constBufferDataSize[i]);
			succeedFlag = true;
		}
	}
	if (!succeedFlag)
	{
		assert(0);
	}
}

void IRender::Release(RESOURCE_HANDLE HANDLE)
{
	gpuBuffer->ReleaseBuffer(HANDLE);
	handle.DeleteHandle(HANDLE);

	constBufferDataName[HANDLE] = "";
	constBufferDataSize[HANDLE] = 0;
	constBufferRootParam[HANDLE] = GRAPHICS_PRAMTYPE_NONE;
	constBufferRangeType[HANDLE] = GRAPHICS_RANGE_TYPE_NONE;
	constBufferHandles[HANDLE] = -1;
}

void IRender::SetConstBufferOnCmdList(PipeLineNames pipeline, bool REMOVE_DATA_FLAG, bool d)
{
	for (int i = 0; i < constBufferRootParam.size(); i++)
	{
		bool lRemoveFlag = true;
		if (!d)
		{
			lRemoveFlag = i != 1 || !REMOVE_DATA_FLAG;
		}

		bool lErrorFlag = constBufferRootParam[i] != GRAPHICS_RANGE_TYPE_NONE;
		if (lRemoveFlag && lErrorFlag)
		{
			UINT lNum = KazRenderHelper::SetBufferOnCmdList(GraphicsRootSignature::Instance()->GetRootParam(renderData.pipelineMgr->GetRootSignatureName(pipeline)), constBufferRangeType[i], constBufferRootParam[i]);

			switch (constBufferRangeType[i])
			{
			case GRAPHICS_RANGE_TYPE_CBV_VIEW:
				renderData.cmdListInstance->cmdList->SetGraphicsRootConstantBufferView(lNum, gpuBuffer->GetBufferData(constBufferHandles[i]).Get()->GetGPUVirtualAddress());
				break;
			case GRAPHICS_RANGE_TYPE_UAV_VIEW:
				renderData.cmdListInstance->cmdList->SetGraphicsRootUnorderedAccessView(lNum, gpuBuffer->GetBufferData(constBufferHandles[i]).Get()->GetGPUVirtualAddress());
				break;
			default:
				break;
			}
		}
	}
}

void IRender::DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData &DATA)
{
	//•`‰æ–½—ß-----------------------------------------------------------------------------------------------------
	renderData.cmdListInstance->cmdList->IASetPrimitiveTopology(DATA.topology);
	renderData.cmdListInstance->cmdList->IASetVertexBuffers(DATA.vertexBufferDrawData.slot, DATA.vertexBufferDrawData.numViews, &DATA.vertexBufferDrawData.vertexBufferView);
	renderData.cmdListInstance->cmdList->IASetIndexBuffer(&DATA.indexBufferView);
	renderData.cmdListInstance->cmdList->DrawIndexedInstanced(
		DATA.drawIndexInstancedData.indexCountPerInstance,
		DATA.drawIndexInstancedData.instanceCount,
		DATA.drawIndexInstancedData.startIndexLocation,
		DATA.drawIndexInstancedData.baseVertexLocation,
		DATA.drawIndexInstancedData.startInstanceLocation
	);
	//•`‰æ–½—ß-----------------------------------------------------------------------------------------------------
}

void IRender::DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData &DATA)
{
	renderData.cmdListInstance->cmdList->IASetPrimitiveTopology(DATA.topology);
	renderData.cmdListInstance->cmdList->IASetVertexBuffers(DATA.vertexBufferDrawData.slot, DATA.vertexBufferDrawData.numViews, &DATA.vertexBufferDrawData.vertexBufferView);
	renderData.cmdListInstance->cmdList->DrawInstanced(
		DATA.drawInstanceData.vertexCountPerInstance,
		DATA.drawInstanceData.instanceCount,
		DATA.drawInstanceData.startVertexLocation,
		DATA.drawInstanceData.startInstanceLocation
	);
}
