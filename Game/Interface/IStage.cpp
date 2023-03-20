#include "IStage.h"

IStage::IStage() :objectSpeed(5.0f)
{
}

IStage::~IStage()
{
}

void IStage::InitBackGround(const KazMath::Color &START_COLOR, const KazMath::Color &END_COLOR, float SPACE, float HEIGHT)
{
	{
		std::array<SpriteVertex, 4>lVert;
		DirectX::XMVECTOR lAdjPos = { 50.0f,0.0f,0.0f };
		lVert[0].pos = { -7680.0f,407.0f,0.0f };
		lVert[0].uv = { 0.0f,0.0f };
		lVert[1].pos = { 7680.0f,407.0f,0.0f };
		lVert[1].uv = { 1.0f,0.0f };
		lVert[2].pos = { -7680.0f,-407.0f,0.0f };
		lVert[2].uv = { 0.0f,1.0f };
		lVert[3].pos = { 7680.0f,-407.0f,0.0f };
		lVert[3].uv = { 1.0f,1.0f };

		GradationData lGradationData;
		lGradationData.firstColor = START_COLOR.ConvertColorRateToXMFLOAT4();
		lGradationData.endColor = END_COLOR.ConvertColorRateToXMFLOAT4();
		for (int i = 0; i < backGround.size(); ++i)
		{
			backGround[i] = std::make_unique<PolygonRender>(lVert);
			RESOURCE_HANDLE lHandle = backGround[i]->CreateConstBuffer(sizeof(GradationData), typeid(GradationData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
			backGround[i]->TransData(&lGradationData, lHandle, typeid(lGradationData).name());
			backGround[i]->data.pipelineName = PIPELINE_NAME_SPRITE_GRADATION;
		}

		backGround[0]->data.transform.pos.z = SPACE;
		backGround[1]->data.transform.pos.x = SPACE;
		backGround[1]->data.transform.rotation.y = 100.0f;
		backGround[2]->data.transform.pos.x = -SPACE;
		backGround[2]->data.transform.rotation.y = 100.0f;
		backGround[3]->data.transform.pos.z = -SPACE;
	}

	{
		std::array<SpriteVertex, 4>lVert;
		DirectX::XMVECTOR adjPos = { 50.0f,0.0f,0.0f };
		lVert[0].pos = DirectX::XMFLOAT3(-800.0f, 0.0f, 700.0f);
		lVert[0].uv = { 0.0f,0.0f };
		lVert[1].pos = DirectX::XMFLOAT3(800.0f, 0.0f, 700.0f);
		lVert[1].uv = { 1.0f,0.0f };
		lVert[2].pos = DirectX::XMFLOAT3(-800.0f, 0.0f, -700.0f);
		lVert[2].uv = { 0.0f,1.0f };
		lVert[3].pos = DirectX::XMFLOAT3(800.0f, 0.0f, -700.0f);
		lVert[3].uv = { 1.0f,1.0f };
	
		{
			topPolygon = std::make_unique<PolygonRender>(lVert);
			RESOURCE_HANDLE lHandle = topPolygon->CreateConstBuffer(sizeof(GradationData), typeid(GradationData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
			topPolygon->data.pipelineName = PIPELINE_NAME_SPRITE_GRADATION;
			GradationData lGradData;
			lGradData.endColor = END_COLOR.ConvertColorRateToXMFLOAT4();
			lGradData.firstColor = END_COLOR.ConvertColorRateToXMFLOAT4();
			topPolygon->TransData(&lGradData, lHandle, typeid(lGradData).name());

			topPolygon->data.transform.pos.y = HEIGHT;
		}

		{
			buttomPolygon = std::make_unique<PolygonRender>(lVert);
			RESOURCE_HANDLE lHandle = buttomPolygon->CreateConstBuffer(sizeof(GradationData), typeid(GradationData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);
			buttomPolygon->data.pipelineName = PIPELINE_NAME_SPRITE_GRADATION;
			GradationData lGradData;
			lGradData.endColor = START_COLOR.ConvertColorRateToXMFLOAT4();
			lGradData.firstColor = START_COLOR.ConvertColorRateToXMFLOAT4();
			buttomPolygon->TransData(&lGradData, lHandle, typeid(lGradData).name());
			buttomPolygon->data.transform.pos.y = -HEIGHT;
		}
	}
}

void IStage::DrawBackGround()
{
	for (int i = 0; i < backGround.size(); ++i)
	{
		backGround[i]->Draw();
	}
	topPolygon->Draw();
	buttomPolygon->Draw();
}
