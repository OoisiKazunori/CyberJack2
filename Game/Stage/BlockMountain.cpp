#include "BlockMountain.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

BlockMountain::BlockMountain()
{

}

void BlockMountain::Init(const InitBlockMountainData &INIT_DATA)
{
	const float L_SCALE = 100.0f;
	const int X_MAX_NUM = INIT_DATA.xRange;
	const int Z_MAX_NUM = 70;

	//それぞれの軸のブロックの配置最大数からマップの真ん中に配置するようにする
	const KazMath::Vec3<float>L_BLOCK_CENTRAL_POS(
		-(static_cast<float>(X_MAX_NUM) * (L_SCALE * 2.0f)) / 2.0f,
		0.0f,
		-(static_cast<float>(Z_MAX_NUM) * (L_SCALE * 2.0f)) / 2.0f
	);


	boxMaxNum = X_MAX_NUM * Z_MAX_NUM;
	boxDataArray.resize(boxMaxNum);


	int lBoxIndex = 0;
	for (int x = 0; x < X_MAX_NUM; ++x)
	{
		for (int z = 0; z < Z_MAX_NUM; ++z)
		{
			boxDataArray[lBoxIndex].transform.pos =
			{
				INIT_DATA.centralPos.x + L_BLOCK_CENTRAL_POS.x + static_cast<float>(x) * (L_SCALE * 2.2f),
				INIT_DATA.centralPos.y,
				INIT_DATA.centralPos.z + L_BLOCK_CENTRAL_POS.z + static_cast<float>(z) * (L_SCALE * 2.2f),
			};


			float lScale = KazMath::Rand(5.0f, 2.0f);
			boxDataArray[lBoxIndex].transform.scale = { lScale,lScale,lScale };

			boxDataArray[lBoxIndex].transform.rotation = INIT_DATA.rotation;
			++lBoxIndex;
		}
	}


	billRender = std::make_unique<ObjModelRender>(true, boxMaxNum, true);
	billRender->data.pipelineName = PIPELINE_NAME_BILL;
	billRender->data.handle = ObjResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "Bill_Model.obj");

	billRender->data.addHandle.handle[0] = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "CiEHwaAUUAAp0ZR.jpg");
	billRender->data.addHandle.paramType[0] = GRAPHICS_PRAMTYPE_TEX;

	fogColor.a = INIT_DATA.fogDesinty;
	instanceBufferHandle = billRender->CreateConstBuffer(sizeof(MatData) * boxMaxNum, typeid(MatData).name(), GRAPHICS_RANGE_TYPE_UAV_VIEW, GRAPHICS_PRAMTYPE_DATA2);
	objectBufferHandle = billRender->CreateConstBuffer(sizeof(ObjectData), typeid(ObjectData).name(), GRAPHICS_RANGE_TYPE_CBV_VIEW, GRAPHICS_PRAMTYPE_DATA);

	billRender->TransData(&objectData, objectBufferHandle, typeid(ObjectData).name());


	DirectX::XMFLOAT3 colorRate = KazMath::Color(29, 19, 72, 255).ConvertColorRateToXMFLOAT3();
	objectData.fogData = { colorRate.x,colorRate.y,colorRate.z,fogColor.a };
	billRender->TransData
	(
		&objectData,
		objectBufferHandle,
		typeid(ObjectData).name()
	);
	billRender->demoFlag = true;
}

void BlockMountain::Update()
{

	std::vector<MatData> lData(boxMaxNum);
	for (int i = 0; i < boxDataArray.size(); ++i)
	{
		boxDataArray[i].transform.pos += -KazMath::Vec3<float>(0.0f, 0.0f, 5.0f);
		if (boxDataArray[i].transform.pos.z <= -(70.0f * 100.0f * 2.2f) / 2.0f)
		{
			boxDataArray[i].transform.pos.z = (70.0f * 100.0f * 2.2f) / 2.0f;
		}

		lData[i].mat = KazMath::CaluMat(boxDataArray[i].transform, CameraMgr::Instance()->GetViewMatrix(), CameraMgr::Instance()->GetPerspectiveMatProjection(), { 0,1,0 }, { 0,0,1 });
	}


	billRender->TransData
	(
		lData.data(),
		instanceBufferHandle,
		typeid(MatData).name()
	);

}

void BlockMountain::Draw()
{
	billRender->Draw();
}
