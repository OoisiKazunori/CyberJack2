#include "BlockParticleStage.h"
#include"../KazLibrary/DirectXCommon/DirectX12Device.h"
#include"../KazLibrary/DirectXCommon/DirectX12CmdList.h"
#include"../KazLibrary/Pipeline/GraphicsRootSignature.h"
#include"../KazLibrary/Pipeline/GraphicsPipeLineMgr.h"
#include"../KazLibrary/Helper/KazRenderHelper.h"
#include"../KazLibrary/RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Buffer/DescriptorHeapMgr.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Buffer/UavViewHandleMgr.h"
#include"../Game/Effect/InstanceMeshParticle.h"
#include"../KazLibrary/Loader/MeshParticleLoader.h"

#include"../KazLibrary/Render/GPUParticleRender.h"

const float BlockParticleStage::PILLAR_PARTICLE_INTERVAL_NUM = 15000.0f;

BlockParticleStage::BlockParticleStage()
{
	for (int i = 0; i < splineParticle.size(); ++i)
	{
		//splineParticle[i] = std::make_unique<SplineParticle>(2.5f);
	}

	//大渦初期化--------------------------------------------


	v = { 40.0f,800.0f,100.0f };
	//{
	//	KazMath::Vec3<float>level = { 100.0f,300.0f,0.0f };
	//	std::vector<KazMath::Vec3<float>> limitPosArray;
	//	for (int i = 0; i < 150; ++i)
	//	{
	//		level.x = cosf(KazMath::AngleToRadian(i * static_cast<int>(v.x))) * v.y;
	//		level.y = sinf(KazMath::AngleToRadian(i * static_cast<int>(v.x))) * v.y;
	//		limitPosArray.push_back(KazMath::Vec3<float>(level.x, level.y, -3000.0f + static_cast<float>(i) * v.z));
	//	}
	//	splineParticle[0]->Init(limitPosArray, false);
	//}
	////大渦初期化--------------------------------------------
	//{
	//	for (int i = 1; i < splineParticle.size(); ++i)
	//	{
	//		std::vector<KazMath::Vec3<float>> limitPosArray;
	//		KazMath::Vec3<float>lStartPos;
	//		lStartPos.y = KazMath::Rand(5000.0f, -5000.0f);

	//		//左から右
	//		if (KazMath::Rand(3, 0) < 2)
	//		{
	//			lStartPos.x = KazMath::Rand(7000.0f, 2000.0f);
	//		}
	//		else
	//		{
	//			lStartPos.x = -KazMath::Rand(7000.0f, 2000.0f);
	//		}
	//		bool lupDownFlag = false;
	//		//上に行くかどうか
	//		if (KazMath::Rand(3, 0) < 2)
	//		{
	//			lupDownFlag = true;
	//		}

	//		for (int i = 0; i < 150; ++i)
	//		{
	//			lStartPos.z = -5000.0f + static_cast<float>(i) * v.z;
	//			limitPosArray.push_back(lStartPos);
	//		}
	//		splineParticle[i]->Init(limitPosArray, false);
	//	}
	//}


	floorResourceHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "lambert1_Base_color.png");

	std::vector<VertexUv> lV = GetPlaneData(floorResourceHandle);

	std::vector<DirectX::XMFLOAT3>lPos;
	std::vector<DirectX::XMFLOAT2>lUv;
	for (int i = 0; i < lV.size(); ++i)
	{
		lPos.push_back(lV[i].pos);
		lUv.push_back(lV[i].uv);
	}



	meshBuffer = std::make_unique<CreateMeshBuffer>(lPos, lUv);
	floorAlpha = 1.0f;


	for (int i = 0; i < floorParticleTransform.size(); ++i)
	{
		if (i < 20)
		{
			floorParticleTransform[i].pos = { 0.0f,700.0f,500.0f + static_cast<float>(i) * 700.0f };
		}
		else
		{
			floorParticleTransform[i].pos = { 0.0f,-700.0f,500.0f + static_cast<float>(i - 20) * 700.0f };
		}
		floorParticleTransform[i].rotation = { 90.0f,0.0f,0.0f };

		InitMeshParticleData lFloorData;
		lFloorData.vertData = meshBuffer->GetBufferData(CreateMeshBuffer::DATA_VERT);
		lFloorData.uvData = meshBuffer->GetBufferData(CreateMeshBuffer::DATA_UV);
		lFloorData.alpha = &floorAlpha;
		lFloorData.textureHandle = floorResourceHandle;
		lFloorData.triagnleData.x = 6;
		lFloorData.triagnleData.y = 0;
		lFloorData.triagnleData.z = 10000;
		lFloorData.triagnleData.w = 2;
		lFloorData.motherMat = &floorParticleMotherMat[i];
		particleArrrayData.emplace_back(lFloorData);
	}

	for (int i = 0; i < floorParticleTransform.size(); ++i)
	{
		particleArrrayData[i].alpha = &floorAlpha;
		particleArrrayData[i].particleScale = { 2.0f,2.0f,2.0f };
		particleArrrayData[i].billboardFlag = false;
	}

	MeshParticleLoadData lData{};
	lData.bias = 0;
	lData.perTriangleNum = 100;
	lData.faceCountNum = 250;

	pillarHandle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "house/" + "House_01.fbx", true);
	for (int i = 0; i < pillarParticleTransform.size() / 2; ++i)
	{
		pillarParticleTransform[i].pos = { -5000.0f,0.0f,-1000.0f + static_cast<float>(i) * PILLAR_PARTICLE_INTERVAL_NUM };
		pillarParticleTransform[i].scale = { 23.5f,25.0f,23.5f };
		RESOURCE_HANDLE lHandle = FbxModelResourceMgr::Instance()->GetResourceData(pillarHandle)->textureHandle[0];
		UINT lFaceCountNum = FbxModelResourceMgr::Instance()->GetResourceData(pillarHandle)->faceCountNum;


		//InstanceMeshParticle::Instance()->AddMeshData(
		//	MeshParticleLoader::Instance()->Load(KazFilePathName::StagePath + "house/" + "House_01.fbx", true, &pillarParticleMotherMat[i], lData)
		//);
	}

	const int L_HALF_NUM = static_cast<int>(pillarParticleTransform.size()) / 2;
	for (int i = L_HALF_NUM; i < pillarParticleTransform.size(); ++i)
	{
		pillarParticleTransform[i].pos = { 5000.0f,0.0f,-1000.0f + static_cast<float>(i) * PILLAR_PARTICLE_INTERVAL_NUM - L_HALF_NUM * PILLAR_PARTICLE_INTERVAL_NUM };
		pillarParticleTransform[i].scale = { 23.5f,25.0f,23.5f };
		RESOURCE_HANDLE lHandle = FbxModelResourceMgr::Instance()->GetResourceData(pillarHandle)->textureHandle[0];
		UINT lFaceCountNum = FbxModelResourceMgr::Instance()->GetResourceData(pillarHandle)->faceCountNum;

		/*	InstanceMeshParticle::Instance()->AddMeshData(
				MeshParticleLoader::Instance()->Load(KazFilePathName::StagePath + "house/" + "House_01.fbx", true, &pillarParticleMotherMat[i], lData)
			);*/
	}




	MeshParticleLoadData lParticleData;
	lParticleData.bias = 100;
	lParticleData.faceCountNum = 21935;
	lParticleData.perTriangleNum = 5;

	InitMeshParticleData lStageMeshParticleData = MeshParticleLoader::Instance()->Load(KazFilePathName::StagePath + "Dungeon_Wall.fbx", false, &transformArrayData[0].GetMat(), lParticleData);
	RESOURCE_HANDLE lHandle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "Dungeon_Wall.fbx");
	lStageMeshParticleData.color = { 0.3f,0.3f,0.3f,1.0f };


	collisionArrrayData.emplace_back(InitMeshCollisionData());
	collisionArrrayData[0].vertData = lStageMeshParticleData.vertData;
	collisionArrrayData[0].vertNumArray = FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertNum;
	collisionArrrayData[0].meshParticleData = lStageMeshParticleData;
	collisionArrrayData[0].motherMat = &transformMatArray[0];
	collisionArrrayData[0].colorData = &colorArrayData[0];

	collisionArrrayData.emplace_back(InitMeshCollisionData());
	collisionArrrayData[1].vertData = lStageMeshParticleData.vertData;
	collisionArrrayData[1].vertNumArray = FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertNum;
	collisionArrrayData[1].meshParticleData = lStageMeshParticleData;
	collisionArrrayData[1].motherMat = &transformMatArray[1];
	collisionArrrayData[1].colorData = &colorArrayData[1];

	for (int i = 0; i < colorArrayData.size(); ++i)
	{
		colorArrayData[i].alpha = 1.0f;
		colorArrayData[i].lightData = { 0,0 };
	}

	for (int i = 0; i < transformArrayData.size(); ++i)
	{
		transformArrayData[i].pos = { 0.0f,0.0f,static_cast<float>(i) * 500.0f + 0.0f };
	}

	galacticParticle = std::make_unique<GalacticParticle>();

	for (int i = 0; i < galacticParticle->blockHitBox.size(); ++i)
	{
		blockHitBox.emplace_back(&galacticParticle->blockHitBox[i]);
	}
}

BlockParticleStage::~BlockParticleStage()
{
	DescriptorHeapMgr::Instance()->Release(outputInitViewHandle);
	DescriptorHeapMgr::Instance()->Release(outputViewHandle);
	DescriptorHeapMgr::Instance()->Release(particleDataViewHandle);
}

void BlockParticleStage::Update()
{

	galacticParticle->Update();

	//KazMath::Vec3<float>level = { 100.0f,300.0f,0.0f };
	//std::vector<KazMath::Vec3<float>> limitPosArray;
	//for (int i = 0; i < 150; ++i)
	//{
	//	level.x = cosf(KazMath::AngleToRadian(i * static_cast<int>(v.x))) * v.y;
	//	level.y = sinf(KazMath::AngleToRadian(i * static_cast<int>(v.x))) * v.y;
	//	limitPosArray.push_back(KazMath::Vec3<float>(level.x, level.y, -1000.0f + static_cast<float>(i) * v.z));
	//}
	//for (int i = 0; i < splineParticle.size(); ++i)
	//{
	//	splineParticle[i]->Init(limitPosArray);
	//}

	/*for (int i = 0; i < splineParticle.size(); ++i)
	{
		splineParticle[i]->Update();
	}*/

	for (int i = 0; i < floorParticleTransform.size(); ++i)
	{
		floorParticleTransform[i].pos.z += -5.0f;
		if (floorParticleTransform[i].pos.z <= -1000.0f)
		{
			floorParticleTransform[i].pos.z = (500.0f + static_cast<float>(FLOOR_PARTICLE_MAX_NUM / 2) * 700.0f) - 1200.0f;
		}
		floorParticleMotherMat[i] = floorParticleTransform[i].GetMat();
	}

	//ImGui::Begin("Block");
	//KazImGuiHelper::InputVec2("Flash", &flash);
	//ImGui::End();

	flash.x += 200.0f;
	if (100000.0f <= flash.x)
	{
		flash.x = -10000.0f;
	}
	if (hitFlag)
	{
		flash.y = 1.0f;
	}
	else
	{
		flash.y = 0.0f;
	}

	//for (int i = 0; i < pillarParticleTransform.size(); ++i)
	//{
	//	pillarParticleModel[i]->updateCommonData.flash.x = flash.x;
	//	pillarParticleModel[i]->updateCommonData.flash.y = flash.y;


	//	pillarParticleTransform[i].pos.z += -5.0f;
	//	if (pillarParticleTransform[i].pos.z <= -10000.0f)
	//	{
	//		pillarParticleTransform[i].pos.z = 12500.0f;
	//	}

	//	pillarParticleMotherMat[i] = pillarParticleTransform[i].GetMat();
	//	pillarParticleModel[i]->Update(false);
	//}




	//パーティクルステージの挙動
	for (int i = 0; i < transformArrayData.size(); ++i)
	{
		transformArrayData[i].pos.z += -0.5f;
		transformMatArray[i] = transformArrayData[i].GetMat();

		//一定の座標になったら透明になる。それ以外は実体化させる
		if (transformArrayData[i].pos.z <= -100.0f)
		{
			colorArrayData[i].alpha += -0.01f;
		}
		else
		{
			colorArrayData[i].alpha += 0.01f;
		}

		//透明になったらループさせる
		if (colorArrayData[i].alpha <= 0.0f)
		{
			transformArrayData[i].pos.z = 800.0f;
		}
		//アルファ固定
		if (1.0f <= colorArrayData[i].alpha)
		{
			colorArrayData[i].alpha = 1.0f;
		}


		colorArrayData[i].lightData.y++;
		if (100 <= colorArrayData[i].lightData.y)
		{
			colorArrayData[i].lightData.y = -100;
		}

		if (KazMath::ConvertSecondToFlame(5) <= flashTimer)
		{
			colorArrayData[i].lightData.x = 1;
			flashTimer = 0;
		}
		else
		{
			++flashTimer;
			colorArrayData[i].lightData.x = 0;
		}
	}



}

void BlockParticleStage::Draw()
{

	galacticParticle->Draw();

}