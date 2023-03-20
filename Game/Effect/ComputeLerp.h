#pragma once
#include"../KazLibrary/Helper/ResouceBufferHelper.h"

/// <summary>
/// メモリ上の二つの値を使って線形補完を行う
/// </summary>
class ComputeLerp
{
public:
	ComputeLerp(const ResouceBufferHelper::BufferData &BASE_DATA, const ResouceBufferHelper::BufferData &LERP_DATA);
	void Compute();

private:
	ResouceBufferHelper computeHelper;
};

