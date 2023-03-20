#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/KazRenderHelper.h"
#include"../Math/KazMath.h"
#include"../Pipeline//GraphicsPipeLineMgr.h"
#include"../Pipeline/GraphicsRootSignature.h"
#include"../Buffer/CreateGpuBuffer.h"

class IRender
{
public:
	IRender();

	RESOURCE_HANDLE CreateConstBuffer(const unsigned int &TRANSMISSION_DATA, const string &ID, const GraphicsRangeType &RANGE, const GraphicsRootParamType &ROOTPARAM);
	void TransData(void *DATA, RESOURCE_HANDLE HANDLE, const std::string &ID);
	void Release(RESOURCE_HANDLE HANDLE);

	KazMath::BaseMatWorldData baseMatWorldData;

	KazRenderHelper::DrawIndexInstanceCommandData drawIndexInstanceCommandData;
private:
	static const int BUFFER_DATA_MAX = 10;
	std::array<std::string, BUFFER_DATA_MAX> constBufferDataName;
	std::array<unsigned int, BUFFER_DATA_MAX> constBufferDataSize;
	std::array<RESOURCE_HANDLE, BUFFER_DATA_MAX> constBufferHandles;
	std::array<GraphicsRootParamType, BUFFER_DATA_MAX> constBufferRootParam;
	std::array<GraphicsRangeType, BUFFER_DATA_MAX> constBufferRangeType;

	HandleMaker handle;

protected:
	void SetConstBufferOnCmdList(PipeLineNames pipeline, bool REMOVE_DATA_FLAG = false, bool d = false);

	KazRenderHelper::RenderInstancesData renderData;
	std::unique_ptr<CreateGpuBuffer> gpuBuffer;
	KazRenderHelper::DrawInstanceCommandData drawInstanceCommandData;



	void DrawIndexInstanceCommand(const KazRenderHelper::DrawIndexInstanceCommandData &DATA);

	void DrawInstanceCommand(const KazRenderHelper::DrawInstanceCommandData &DATA);
};