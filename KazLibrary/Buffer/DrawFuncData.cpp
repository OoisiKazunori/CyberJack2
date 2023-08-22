#include "DrawFuncData.h"
#include "../Raytracing/Blas.h"
#include "Raytracing/BlasDataContainer.h"

void DrawFuncData::DrawCallData::SetupRaytracing(bool arg_isOpaque)
{

	/*-- ���C�g���[�V���O�̏����֐� --*/

	//Blas���\�z�B
	m_raytracingData.m_blas = Raytracing::BlasDataContainer::Instance()->SetBlas(arg_isOpaque, m_modelVertDataHandle, materialBuffer);

	//���C�g�����Z�b�g�A�b�v�ς݂̏�Ԃɂ���B
	m_raytracingData.m_isRaytracingInitialized = true;

	//���C�g����L���ɂ���B
	m_raytracingData.m_isRaytracingEnable = true;

}

void DrawFuncData::ExcuteIndirectArgumentData::GenerateArgumentBuffer()
{
}
