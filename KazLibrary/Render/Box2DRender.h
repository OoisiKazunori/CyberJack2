#pragma once
#include"../KazLibrary/DirectXCommon/Base.h"
#include"../Render/RenderData.h"
#include"../Render/IRender.h"

class Box2DRender : public IRender
{
public:
	Box2DRender();
	void Draw();

	Box2DRenderData data;
private:
	std::array<SpriteVertex, 5> vertices;

	PipeLineNames pipeline;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	ConstBufferData constMap;
	DirectX::XMMATRIX motherMat;
	RESOURCE_HANDLE vertexBufferHandle;
	RESOURCE_HANDLE indexBufferHandle;
	RESOURCE_HANDLE constBufferHandle;

	BUFFER_SIZE vertByte;

	enum
	{
		LEFT_UP,
		LEFT_DOWN,
		RIGHT_DOWN,
		RIGHT_UP,
	};

	UINT indicesNum;

private:
};
