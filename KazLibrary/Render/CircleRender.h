#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include"../Math/KazMath.h"
#include"../Helper/DirtyFlag.h"
#include"../Render/RenderData.h"

class CircleRender: public IRender
{
public:
	CircleRender();
	void Draw();

	CircleDrawData data;
private:
	PipeLineNames pipeline;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	ConstBufferData constMap;
	DirectX::XMMATRIX motherMat;
	RESOURCE_HANDLE vertexBufferHandle;
	RESOURCE_HANDLE indexBufferHandle;
	RESOURCE_HANDLE constBufferHandle;


	UINT indicesNum;
private:
};

