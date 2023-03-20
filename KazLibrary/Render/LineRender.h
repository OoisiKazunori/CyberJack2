#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include"../Math/KazMath.h"
#include"../Helper/DirtyFlag.h"
#include"../Render/RenderData.h"

class LineRender :public IRender
{
public:
	LineRender(int INSTANCE_NUM = 1);
	~LineRender();

	void Draw();

	LineDrawData data;
private:
	std::array<LineVertex, 2>vertices;
	UINT vertByte;

	RESOURCE_HANDLE vertexBufferHandle;
	RESOURCE_HANDLE constBufferHandle;
};


class Line2DRender :public IRender
{
public:
	Line2DRender();
	void Draw();

	LineDrawData data;
private:
	PipeLineNames pipeline;
	std::array<LineVertex, 2>vertices;
	UINT vertByte;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	std::array<unique_ptr<DirtySet>, 2> positionDirtyFlag;

	short vertexBufferHandle;
	short indexBufferHandle;
	short constBufferHandle;


	unique_ptr<DirtySet> cameraViewDirtyFlag, cameraProjectionDirtyFlag, cameraBillBoardDirtyFlag;
};

