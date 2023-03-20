#include "ComputeLerp.h"

ComputeLerp::ComputeLerp(const ResouceBufferHelper::BufferData &BASE_DATA, const ResouceBufferHelper::BufferData &LERP_DATA)
{
	computeHelper.SetBuffer(BASE_DATA, GRAPHICS_PRAMTYPE_DATA);
	computeHelper.SetBuffer(LERP_DATA, GRAPHICS_PRAMTYPE_DATA2);
}

void ComputeLerp::Compute()
{
	//computeHelper.Compute(, {});
}
