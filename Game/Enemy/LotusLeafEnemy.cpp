#include "LotusLeafEnemy.h"

LotusLeafEnemy::LotusLeafEnemy()
{
	m_model = DrawFuncData::SetDefferdRenderingModel(ModelLoader::Instance()->Load("Resource/Enemy/LotusLeaf/", "LotusLeaf.gltf"));
}

void LotusLeafEnemy::Init(const KazMath::Transform3D* arg_playerTransform, const EnemyGenerateData& GENERATE_DATA, bool DEMO_FLAG)
{
	iEnemy_EnemyStatusData->oprationObjData->Init(1, "LotusLeaf");
	m_playerTransform = arg_playerTransform;
}

void LotusLeafEnemy::Finalize()
{
}

void LotusLeafEnemy::Update()
{

	m_transform = *m_playerTransform;
	m_transform.pos += KazMath::Vec3<float>(0,0,50);

	DrawFunc::DrawModelInRaytracing(m_model, m_transform, DrawFunc::NONE);
}

void LotusLeafEnemy::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	DrawFunc::DrawModel(m_model, m_transform);
	arg_rasterize.ObjectRender(m_model);
	for (auto& index : m_model.m_raytracingData.m_blas) {
		arg_blasVec.Add(index, m_transform.GetMat());
	}
}
