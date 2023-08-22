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

DrawFuncData::DrawCallData DrawFuncData::SetRaytracingData(const ModelInfomation& MODEL_DATA, const PipelineGenerateData& PIPELINE_DATA)
{
	DrawCallData lDrawCallData;

	//頂点情報
	lDrawCallData.m_modelVertDataHandle = MODEL_DATA.modelVertDataHandle;
	lDrawCallData.drawMultiMeshesIndexInstanceCommandData = VertexBufferMgr::Instance()->GetVertexIndexBuffer(MODEL_DATA.modelVertDataHandle).index;
	lDrawCallData.drawCommandType = VERT_TYPE::MULTI_MESHED;
	for (auto& obj : MODEL_DATA.modelData)
	{
		lDrawCallData.materialBuffer.emplace_back(obj.materialData.textureBuffer);
	}

	lDrawCallData.SetupRaytracing(true);

	return lDrawCallData;
};
