#include "ChildOfEdenStage.h"

ChildOfEdenStage::ChildOfEdenStage() :m_skydormScale(1000.0f)
{
	m_drawSkydorm = DrawFuncData::SetDrawGLTFIndexMaterialData(
		*ModelLoader::Instance()->Load(KazFilePathName::StagePath + "Skydorm/", "skydome.gltf"),
		DrawFuncData::GetModelShader()
	);
	m_skydormTransform.scale = { m_skydormScale,m_skydormScale,m_skydormScale };
}

void ChildOfEdenStage::Update()
{
}

void ChildOfEdenStage::Draw(DrawingByRasterize& arg_rasterize)
{
	//DrawFunc::DrawModel(m_drawSkydorm, m_skydormTransform);
	//arg_rasterize.ObjectRender(m_drawSkydorm);
}