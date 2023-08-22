#include "DrawFuncData.h"
#include "../Raytracing/Blas.h"
#include "Raytracing/BlasDataContainer.h"

void DrawFuncData::DrawCallData::SetupRaytracing(bool arg_isOpaque)
{

	/*-- レイトレーシングの準備関数 --*/

	//Blasを構築。
	m_raytracingData.m_blas = Raytracing::BlasDataContainer::Instance()->SetBlas(arg_isOpaque, m_modelVertDataHandle, materialBuffer);

	//レイトレがセットアップ済みの状態にする。
	m_raytracingData.m_isRaytracingInitialized = true;

	//レイトレを有効にする。
	m_raytracingData.m_isRaytracingEnable = true;

}

void DrawFuncData::ExcuteIndirectArgumentData::GenerateArgumentBuffer()
{
}
