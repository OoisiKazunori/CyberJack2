#pragma once
#include"../DirectXCommon/Base.h"
#include"../Helper/ISinglton.h"
#include"../Helper/KazHelper.h"
#include"../Helper/KazBufferHelper.h"
#include"../Buffer/CreateGpuBuffer.h"

class UavViewHandleMgr :public ISingleton<UavViewHandleMgr>
{
public:
	UavViewHandleMgr();
	void Init();
	RESOURCE_HANDLE GetHandle();

private:
	HandleMaker handle;

};