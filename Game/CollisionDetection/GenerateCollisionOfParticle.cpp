#include"GenerateCollisionOfParticle.h"

GenerateCollisionOfParticle::GenerateCollisionOfParticle(const InitCollisionOfParticleData &INIT_DATA)
{
	computeHelper.SetBuffer(INIT_DATA.hitBoxData, GRAPHICS_PRAMTYPE_DATA);
	computeHelper.SetBuffer(INIT_DATA.particleData, GRAPHICS_PRAMTYPE_DATA2);
	computeHelper.SetBuffer(INIT_DATA.stackParticleHitBoxData, GRAPHICS_PRAMTYPE_DATA3);

	RESOURCE_HANDLE lCommonBufferHandle = computeHelper.CreateBuffer(sizeof(CommonData), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA4, 1);

	CommonData lCommonData;
	lCommonData.particleRadius = INIT_DATA.particleRadius;
	lCommonData.meshHitBoxMaxNum = INIT_DATA.meshHitBoxNum;
	lCommonData.meshHitBoxRadius = INIT_DATA.meshHitBoxRadius;
	computeHelper.TransData(lCommonBufferHandle, &lCommonData, sizeof(CommonData));
}

void GenerateCollisionOfParticle::Compute()
{
	computeHelper.StackToCommandListAndCallDispatch(PIPELINE_COMPUTE_NAME_HITBOX_ID_ATTACH_TO_PARTICLE, { 1000,1,1 }, -1);
}