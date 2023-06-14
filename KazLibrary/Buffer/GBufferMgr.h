#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"

class GBufferMgr :public ISingleton<GBufferMgr>
{
public:
	enum BufferType
	{
		NONE = -1,
		ALBEDO,
		NORMAL
	};
	GBufferMgr();
	KazBufferHelper::BufferData GetBuffer(BufferType arg_type);


private:
	std::vector<KazBufferHelper::BufferData>m_gBufferArray;
};

