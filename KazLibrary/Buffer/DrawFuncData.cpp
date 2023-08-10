#include "DrawFuncData.h"
#include "../Raytracing/Blas.h"

void DrawFuncData::DrawCallData::SetupRaytracing(bool arg_isOpaque)
{

	/*-- レイトレーシングの準備関数 --*/

	//Blasを構築。
	const int BLAS_COUNT = static_cast<int>(materialBuffer.size());
	m_raytracingData.m_blas.resize(BLAS_COUNT);
	for (int counter = 0; counter < BLAS_COUNT; ++counter) {
		m_raytracingData.m_blas[counter] = std::make_shared<Raytracing::Blas>(arg_isOpaque, m_modelVertDataHandle, counter, materialBuffer[counter][0].bufferWrapper->GetViewHandle());
	}

	//レイトレがセットアップ済みの状態にする。
	m_raytracingData.m_isRaytracingInitialized = true;

	//レイトレを有効にする。
	m_raytracingData.m_isRaytracingEnable = true;

}

void DrawFuncData::ExcuteIndirectArgumentData::GenerateArgumentBuffer()
{
}
