#include "DrawFuncData.h"
#include "../Raytracing/Blas.h"

void DrawFuncData::DrawCallData::SetupRaytracing(bool arg_isOpaque)
{

	/*-- ���C�g���[�V���O�̏����֐� --*/

	//Blas���\�z�B
	const int BLAS_COUNT = static_cast<int>(materialBuffer.size());
	m_raytracingData.m_blas.resize(BLAS_COUNT);
	for (int counter = 0; counter < BLAS_COUNT; ++counter) {
		m_raytracingData.m_blas[counter] = std::make_shared<Raytracing::Blas>(arg_isOpaque, m_modelVertDataHandle, counter, materialBuffer[counter][0].bufferWrapper->GetViewHandle());
	}

	//���C�g�����Z�b�g�A�b�v�ς݂̏�Ԃɂ���B
	m_raytracingData.m_isRaytracingInitialized = true;

	//���C�g����L���ɂ���B
	m_raytracingData.m_isRaytracingEnable = true;

}

void DrawFuncData::ExcuteIndirectArgumentData::GenerateArgumentBuffer()
{
}
