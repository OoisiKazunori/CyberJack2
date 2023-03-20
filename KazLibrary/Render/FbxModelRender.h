#pragma once
#include"../DirectXCommon/Base.h"
#include"../Render/IRender.h"
#include"../Helper/DirtyFlag.h"
#include"../Render/RenderData.h"
#include"../Loader/FbxModelResourceMgr.h"

class FbxModelRender :public IRender
{
public:
	FbxModelRender();
	void Draw(bool DRAW_FLAG = true);
	void CaluMat();

	void ReleaseSkining();
	FbxModelData data;
	FbxTime currentTime;

	DirectX::XMMATRIX motherMat;
	DirectX::XMMATRIX motherBoneMat;
private:

	UINT vertByte;
	UINT indexByte;

	short vertexBufferHandle;
	short indexBufferHandle;

	std::array<RESOURCE_HANDLE, 2> constBufferHandle;
	std::shared_ptr<FbxResourceData> resourceData;

	FbxTime frameTime;
	FbxTime startTime;
	FbxTime endTime;


	bool removeSkining;
};

