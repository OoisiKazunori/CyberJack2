#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include"../Render/RenderData.h"
#include"../Helper/DirtyFlag.h"

class BoxPolygonRender :public IRender
{
public:
	BoxPolygonRender(bool INSTANCE_FLAG = false, int INSTANCE_NUM = 1);
	~BoxPolygonRender();
	void Draw();

	DirectX::XMMATRIX GetMotherMatrix();
	PolygonDrawData data;
private:
	DirectX::XMMATRIX motherMatrix;

	RESOURCE_HANDLE constBufferHandle;
	bool instanceFlag;
};

