#pragma once
#include"../KazLibrary/Helper/ISinglton.h"
#include"../KazLibrary/Helper/KazBufferHelper.h"

class ShaderRandomTable :public ISingleton<ShaderRandomTable>
{
public:
	ShaderRandomTable();

	const KazBufferHelper::BufferData& GetBuffer(GraphicsRootParamType arg_rootparam);


private:
	static const int RANDOM_TABLE_NUM = 1024 * 100;
	KazBufferHelper::BufferData m_shaderRandomTableBuffer;
};

