#include "DebugMeshParticle.h"
#include"../Camera/CameraMgr.h"
#include"../Input/KeyBoradInputManager.h"
#include"../Loader/FbxModelResourceMgr.h"
#include"../Loader/MeshParticleLoader.h"
#include"../Helper/ResourceFilePass.h"
#include"../RenderTarget/RenderTargetStatus.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../Game/Stage/BlockParticleStage.h"
#include"../Game/Stage/FirstStage.h"
#include"../Game/Stage/RezStage.h"
#include<cmath>
#include<iostream>
#include<vector>

#include"../KazLibrary/Input/ControllerInputManager.h"


DebugMeshParticleScene::DebugMeshParticleScene() :
	sphereModel(FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "sphere.fbx")),
	enemyModel(FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "Move/" + "MoveEnemy_Model.fbx", true)),
	summonModel(FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "Summon/" + "SummonEnemy_anim.fbx")),
	cubeModel(FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "testCube.fbx"))
{
	rendertarget = std::make_unique<GameRenderTarget>(KazMath::Color(0, 0, 0, 255));
	mainRenderTarget.data.handleData = rendertarget->GetGameRenderTargetHandle();
	mainRenderTarget.data.pipelineName = PIPELINE_NAME_SPRITE_NOBLEND;
	mainRenderTarget.data.transform.pos = { WIN_X / 2.0f,WIN_Y / 2.0f };


	filePass[0] = KazFilePathName::EnemyPath + "BattleShip/" + "BattleshipEnemy_Head_anim.fbx";
	filePass[1] = KazFilePathName::EnemyPath + "Bike/" + "BikeEnemy_anim.fbx";
	filePass[2] = KazFilePathName::EnemyPath + "MisileEnemy/" + "Gunner_Switch_anim.fbx";
	filePass[3] = KazFilePathName::EnemyPath + "Summon/" + "SummonEnemy_anim.fbx";
	filePass[4] = KazFilePathName::EnemyPath + "Move/" + "MoveEnemy_Model.obj";
	filePass[5] = KazFilePathName::EnemyPath + "PopEnemy/" + "PopEnemy_Model.obj";

	for (int i = 0; i < meshEmitter.size(); ++i)
	{
		std::string lPass;
		lPass += filePass[i][filePass[i].size() - 3];
		lPass += filePass[i][filePass[i].size() - 2];
		lPass += filePass[i][filePass[i].size() - 1];

		if (lPass == "obj")
		{
			RESOURCE_HANDLE lHandle = ObjResourceMgr::Instance()->LoadModel(filePass[i]);
			meshEmitter[i] = std::make_unique<MeshParticleEmitter>(ObjResourceMgr::Instance()->GetResourceData(lHandle).vertices);
		}
		else
		{
			RESOURCE_HANDLE lHandle = FbxModelResourceMgr::Instance()->LoadModel(filePass[i]);
			meshEmitter[i] = std::make_unique<MeshParticleEmitter>(FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertFloat4Data);
		}
	}
	meshIndex = 0;
	model.data.handle = FbxModelResourceMgr::Instance()->LoadModel(filePass[meshIndex]);
	model.data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	model.data.removeMaterialFlag = true;
	model.data.colorData.color = { 255,255,255,255 };
	model.data.transform.scale = { 1.0f,1.0f,1.0f };


	std::array<Vertex, 4>vertices;
	std::array<USHORT, 6> indices;
	indices = KazRenderHelper::InitIndciesForPlanePolygon();
	KazRenderHelper::InitVerticesPos(&vertices[0].pos, &vertices[1].pos, &vertices[2].pos, &vertices[3].pos, { 0.5f,0.5f });
	KazRenderHelper::InitUvPos(&vertices[0].uv, &vertices[1].uv, &vertices[2].uv, &vertices[3].uv);

	handle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::TestPath + "tex.png");

	KazMath::Vec2<int>lTexSize
	(
		static_cast<int>(TextureResourceMgr::Instance()->GetTextureSize(handle).Width),
		static_cast<int>(TextureResourceMgr::Instance()->GetTextureSize(handle).Height))
		;

	KazMath::Vec2<float> lLeftUp, lRightDown;
	lLeftUp = { 0.0f,0.0f };
	lRightDown = { 1.0f,1.0f };
	//サイズ変更
	std::array<KazMath::Vec2<float>, 4>lVert;
	lVert = KazRenderHelper::ChangePlaneScale(lLeftUp, lRightDown, { 1.0f,1.0f }, { 0.5f,0.5f }, lTexSize);


	std::vector<VertexUv>lVertArray;
	modelHandle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "Move/" + "MoveEnemy_Model.fbx", true);
	lVertArray = FbxModelResourceMgr::Instance()->GetResourceData(modelHandle)->vertUvData;
	handle = FbxModelResourceMgr::Instance()->GetResourceData(modelHandle)->textureHandle[0];
	//重複ありの三角形
	RESOURCE_HANDLE lHandle = TextureResourceMgr::Instance()->LoadGraph(KazFilePathName::StagePath + "lambert1_Base_color.png");
	texMotherMat = texTransform.GetMat();
	texParticle = std::make_unique<TextureParticle>(lVertArray, &texMotherMat, handle, 0.5f, 50, 500);

	splineParticle = std::make_unique<SplineParticle>(1.0f);
	for (int i = 0; i < 6; ++i)
	{
		limitPos.emplace_back(KazMath::Vec3<float>(0.0f, 0.0f, -300.0f + static_cast<float>(i) * 400.0f));
	}


	RESOURCE_HANDLE lSphereHandle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "sphere.fbx");
	//collision = std::make_unique<GPUCollisionDetectionOfMeshHitBoxAndSphere>(FbxModelResourceMgr::Instance()->GetResourceData(lSphereHandle)->vertFloat4Data, 0.05f);
	//collision->Init(&meshMat);

	meshTransform.scale = { 50.0f,50.0f,50.0f };
	meshTransform.pos = { -100.0f,90.0f,100.0f };

	particleWall = std::make_unique<ParticleWall>();

	vec1 = std::make_unique<LineRender>();
	vec2 = std::make_unique<LineRender>();



	particleRender = std::make_unique<GPUParticleRender>();
	meshParticle = std::make_unique<InstanceMeshParticle>(particleRender.get());




	RESOURCE_HANDLE lEnemyHandle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::EnemyPath + "Move/" + "MoveEnemy_Model.fbx", true);
	RESOURCE_HANDLE lSummonHandle = FbxModelResourceMgr::Instance()->LoadModel(filePass[3]);
	sphereModelMat = DirectX::XMMatrixIdentity();



	InitMeshParticleData lEnemyMeshData;
	lEnemyMeshData.vertData = enemyModel.GetBufferData(CreateMeshBuffer::DATA_VERT);
	lEnemyMeshData.uvData = enemyModel.GetBufferData(CreateMeshBuffer::DATA_UV);

	lEnemyMeshData.triagnleData =
	{
		FbxModelResourceMgr::Instance()->GetResourceData(lEnemyHandle)->vertNum,
		100,
		50,
		110
	};


	InitMeshParticleData lSphereMeshData;
	lSphereMeshData.vertData = sphereModel.GetBufferData(CreateMeshBuffer::DATA_VERT);
	//lSphereMeshData.uvData = lSphereModel.GetBufferData(CreateMeshBuffer::DATA_UV);
	lSphereMeshData.motherMat = &sphereModelMat;
	lSphereMeshData.color = { 255,0,0,255 };
	lSphereMeshData.triagnleData =
	{
		FbxModelResourceMgr::Instance()->GetResourceData(lSphereHandle)->vertNum,
		100,
		50,
		FbxModelResourceMgr::Instance()->GetResourceData(lSphereHandle)->faceCountNum
	};

	InitMeshParticleData lSummonMeshData;
	lSummonMeshData.vertData = summonModel.GetBufferData(CreateMeshBuffer::DATA_VERT);
	lSummonMeshData.uvData = summonModel.GetBufferData(CreateMeshBuffer::DATA_UV);
	//lSummonMeshData.motherMat = &summonModelMat;
	lSummonMeshData.triagnleData =
	{
		FbxModelResourceMgr::Instance()->GetResourceData(lSummonHandle)->vertNum,
		100,
		50,
		1000
		//FbxModelResourceMgr::Instance()->GetResourceData(lSummonHandle)->faceCountNum
	};


	al = 1.0f;
	std::vector<InitMeshParticleData>lInitData;
	for (int i = 0; i < 10; ++i)
	{
		enemyModelMat[i] = KazMath::CaluWorld(KazMath::Transform3D(KazMath::Vec3<float>(10.0f, 0.0f, 0.0f + 10.0f * static_cast<float>(i)), { 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }), { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f });
		summonModelMat[i] = KazMath::CaluWorld(KazMath::Transform3D(KazMath::Vec3<float>(-10.0f, 0.0f, 0.0f + 10.0f * static_cast<float>(i)), { 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }), { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f });

		MeshParticleLoadData lData;
		lData.bias = 70;
		lData.faceCountNum = 1000;
		lData.perTriangleNum = 50;

		lInitData.emplace_back(MeshParticleLoader::Instance()->Load(KazFilePathName::EnemyPath + "Move/" + "MoveEnemy_Model.fbx", true, &enemyModelMat[i], lData));
		lInitData.emplace_back(MeshParticleLoader::Instance()->Load(filePass[3], false, &summonModelMat[i], lData));

		lInitData[i].alpha = &al;
		meshParticle->AddMeshData(lInitData[i]);
	}


	MeshParticleLoadData lData;
	lData.bias = 70;
	lData.faceCountNum = 21935;
	lData.perTriangleNum = 5;

	enemyModelMat[0] = KazMath::CaluWorld(KazMath::Transform3D(KazMath::Vec3<float>(10.0f, 0.0f, 0.0f), { 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }), { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f });
	enemyModelMat[1] = KazMath::CaluWorld(KazMath::Transform3D(KazMath::Vec3<float>(10.0f, 0.0f, 500.0f), { 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }), { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f });

	InitMeshParticleData lStageMeshParticleData = MeshParticleLoader::Instance()->Load(KazFilePathName::StagePath + "Dungeon_Wall.fbx", false, &enemyModelMat[0], lData);

	drawInstanceMeshParticleFlag = false;

	std::vector<ResouceBufferHelper::BufferData>lVertArrayData;
	std::vector<UINT>lVertNumArrayData;
	lVertArrayData.emplace_back(sphereModel.GetBufferData(CreateMeshBuffer::DATA_VERT));
	lVertNumArrayData.emplace_back(FbxModelResourceMgr::Instance()->GetResourceData(lSummonHandle)->vertNum);

	lStageMeshParticleData.color = { 0.2f,0.2f,0.2f,1.0f };

	color1.lightData.x = 0;
	color1.lightData.y = -100;
	color1.alpha = 1.0f;

	color2.lightData.x = 0;
	color2.lightData.y = -100;
	color2.alpha = 0.5f;

	colorArrayData.emplace_back(&color1);
	colorArrayData.emplace_back(&color2);

	{
		RESOURCE_HANDLE lHandle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::StagePath + "Dungeon_Wall.fbx");

		std::vector<InitMeshCollisionData> lInitCollisionData;
		lInitCollisionData.push_back(InitMeshCollisionData());
		lInitCollisionData[0].vertData = lStageMeshParticleData.vertData;
		lInitCollisionData[0].vertNumArray = FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertNum;
		lInitCollisionData[0].meshParticleData = lStageMeshParticleData;
		lInitCollisionData[0].motherMat = &enemyModelMat[0];
		lInitCollisionData[0].colorData = colorArrayData[0];

		lInitCollisionData.push_back(InitMeshCollisionData());
		lInitCollisionData[1].vertData = lStageMeshParticleData.vertData;
		lInitCollisionData[1].vertNumArray = FbxModelResourceMgr::Instance()->GetResourceData(lHandle)->vertNum;
		lInitCollisionData[1].meshParticleData = lStageMeshParticleData;
		lInitCollisionData[1].motherMat = &enemyModelMat[1];
		lInitCollisionData[1].colorData = colorArrayData[1];

		s1 = Sphere(&collisionPos, 15.0f);
		s2 = Sphere(&collisionPos2, 15.0f);
		s3 = Sphere(&collisionPos3, 15.0f);

		hitBoxArray.push_back(&s1);
		hitBoxArray.push_back(&s2);
		hitBoxArray.push_back(&s3);

		meshCollision = std::make_unique<InstanceMeshCollision>(lInitCollisionData, hitBoxArray);
	}
}

DebugMeshParticleScene::~DebugMeshParticleScene()
{
}

void DebugMeshParticleScene::Init()
{
	cameraWork.Init();

	triangelPosArray[0] = { -30.0f,0.0f,0.0f };
	triangelPosArray[1] = { 30.0f,0.0f,0.0f };
	triangelPosArray[2] = { 0.0f,30.0f,0.0f };


	hitBoxTrianglePosArray[0] = { -30.0f,0.0f,0.0f };
	hitBoxTrianglePosArray[1] = { 30.0f,0.0f,0.0f };
	hitBoxTrianglePosArray[2] = { 0.0f,30.0f,0.0f };
	pointPosR.data.color = { 0,200,0,255 };

	for (int i = 0; i < meshEmitter.size(); ++i)
	{
		meshEmitter[i]->Init(&motherMat);
	}

	prevDeadParticleFlag = false;
	deadParticleFlag = false;

	texParticle->Init();



	//メッシュパーティクル初期化--------------------------------------------
	const float lSize = 1.0f;

	Vertex lVertices[] =
	{
		//x,y,z	法線	u,v
	//前
	{{-lSize, -lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{-lSize,  lSize, -lSize},{}, {0.0f,0.0f}},		//左上
	{{ lSize, -lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{ lSize,  lSize, -lSize}, {}, {1.0f,0.0f}},		//右上

	//後
	{{-lSize,-lSize,  lSize}, {}, {0.0f,1.0f}},		//左下
	{{-lSize, lSize,  lSize}, {},{0.0f,0.0f}},		//左上
	{{ lSize,-lSize,  lSize},{},{1.0f,1.0f}},		//右下
	{{ lSize, lSize,  lSize},{}, {1.0f,0.0f}},		//右上

	//左
	{{-lSize,-lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{-lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},		//左上
	{{-lSize, lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{-lSize, lSize,  lSize}, {}, {1.0f,0.0f}},		//右上

	//右
	{{lSize,-lSize, -lSize},{}, {0.0f,1.0f}},		//左下
	{{lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},		//左上
	{{lSize, lSize, -lSize}, {}, {1.0f,1.0f}},		//右下
	{{lSize, lSize,  lSize}, {}, {1.0f,0.0f}},		//右上

	//下
	{{ lSize, -lSize, lSize}, {}, {0.0f,1.0f}},		//左下
	{{ lSize, -lSize,-lSize}, {}, {0.0f,0.0f}},		//左上
	{{-lSize, -lSize, lSize}, {}, {1.0f,1.0f}},		//右下
	{{-lSize, -lSize,-lSize}, {}, {1.0f,0.0f}},	//右上

	//上
	{{ lSize, lSize, lSize}, {}, {0.0f,1.0f}},			//左下
	{{ lSize, lSize,-lSize}, {}, {0.0f,0.0f}},			//左上
	{{-lSize, lSize, lSize}, {}, {1.0f,1.0f}},			//右下
	{{-lSize, lSize,-lSize}, {}, {1.0f,0.0f}}			//右上
	};

	const unsigned short lIndices[] =
	{
		//前
		0,1,2,		//三角形1つ目
		2,1,3,		//三角形2つ目
		//後
		4,6,5,		//三角形3つ目
		6,7,5,		//三角形4つ目
		//左
		8,9,10,		//三角形1つ目
		10,9,11,	//三角形2つ目
		//右
		12,14,13,	//三角形1つ目
		13,14,15,	//三角形2つ目
		//下
		16,18,17,	//三角形1つ目
		17,18,19,	//三角形2つ目
		//上
		20,21,22,	//三角形3つ目
		22,21,23	//三角形4つ目
	};


	particleAngle.resize(12);

	particleHitBox.resize(12);
	particleHitBox2.resize(12);
	baseParticleHitBox.resize(12);
	baseParticleHitBox2.resize(12);

	blockPos = { 0.0f,0.0f,0.0f };
	blockPos2 = { 2.0f,0.0f,0.0f };
	InitMeshParticle(blockPos, particleHitBox, baseParticleHitBox);
	InitMeshParticle(blockPos2, particleHitBox2, baseParticleHitBox2);
	//メッシュパーティクル初期化--------------------------------------------

	for (int surfaceIndex = 0; surfaceIndex < baseParticleHitBox.size(); ++surfaceIndex)
	{
		for (int i = 0; i < baseParticleHitBox[surfaceIndex].size(); ++i)
		{
			//particleLarpPos[surfaceIndex][i] = baseParticleHitBox[surfaceIndex][i]->data.transform.pos;
		}
	}

	for (int surfaceIndex = 0; surfaceIndex < baseParticleHitBox2.size(); ++surfaceIndex)
	{
		for (int i = 0; i < baseParticleHitBox2[surfaceIndex].size(); ++i)
		{
			//particleLarpPos2[surfaceIndex][i] = baseParticleHitBox2[surfaceIndex][i]->data.transform.pos;
		}
	}

	meshPos = { 1.0f,50.0f,0.0f };

	larpVel = 0.5f;


	circleModelR.data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "sphere.fbx");
	circleModelR.data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	circleModelR.data.removeMaterialFlag = true;

	circleModelR2.data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "sphere.fbx");
	circleModelR2.data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	circleModelR2.data.removeMaterialFlag = true;


	modelHitBox = std::make_unique<FbxModelRender>();
	modelHitBox->data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::TestPath + "sphere.fbx");
	modelHitBox->data.pipelineName = PIPELINE_NAME_COLOR_WIREFLAME;
	modelHitBox->data.removeMaterialFlag = true;
	modelHitBox->data.colorData.color.a = 50;

	minPos = std::make_unique<BoxPolygonRender>();
	minPos->data.color.color = { 255,0,0,255 };
	maxPos = std::make_unique<BoxPolygonRender>();
	maxPos->data.color.color = { 0,255,0,255 };


	meshCollision->Init(particleRender.get());



}

void DebugMeshParticleScene::Finalize()
{
	rendertarget->Finalize();
}

void DebugMeshParticleScene::Input()
{
}

void DebugMeshParticleScene::Update()
{

	eyePos = cameraWork.GetEyePos();
	targetPos = cameraWork.GetTargetPos();
	CameraMgr::Instance()->CameraSetting(60.0f, 100000.0f, 0);
	CameraMgr::Instance()->Camera(eyePos, targetPos, { 0.0f,1.0f,0.0f });

	KazMath::Vec3<float>lPl(0.0f, 0.0f, 0.0f);
	cameraWork.Update(KazMath::Vec2<float>(0.0f, 0.0f), &lPl, true);


	//デバック用のGUI
	ImGui::Begin("MeshParticle");
	ImGui::Checkbox("DrawGrid", &drawGridFlag);
	ImGui::Checkbox("CheckCPUParticle", &cpuCheckParticleFlag);
	ImGui::Checkbox("CheckGPUParticle", &gpuCheckParticleFlag);
	ImGui::Checkbox("CheckTextureParticle", &textureParticleFlag);
	ImGui::Checkbox("CheckSplineParticle", &splineParticleFlag);
	ImGui::Checkbox("CheckPerlinNoise", &perlinNoizeFlag);
	ImGui::Checkbox("CheckTriangleAndSphereHitBox", &cpuCheckTriangleAndSphereHitBoxFlag);
	ImGui::Checkbox("CheckCPUMeshParticleAndSphereHitBox", &cpuCheckMeshParticleAndSphereHitBoxFlag);
	ImGui::Checkbox("CheckGPUHitBox", &gpuCheckHitBoxFlag);
	ImGui::Checkbox("InstanceMeshParticle", &drawInstanceMeshParticleFlag);
	ImGui::Checkbox("MeshHitBox", &drawMeshHitBoxFlag);
	if (cpuCheckParticleFlag)
	{
		if (ImGui::TreeNode("TrianglePosArray"))
		{
			KazImGuiHelper::InputVec3("Pos1", &triangelPosArray[0]);
			KazImGuiHelper::InputVec3("Pos2", &triangelPosArray[1]);
			KazImGuiHelper::InputVec3("Pos3", &triangelPosArray[2]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Particle"))
		{
			ImGui::SliderInt("ParticleNum", &drawParticleNum, 0, PARTICLE_MAX_NUM);
			ImGui::SliderInt("ParticleBias", &particleBias, 0, PARTICLE_MAX_BIAS);
			ImGui::TreePop();
		}
		initFlag = ImGui::Button("SetParticle");
	}
	else if (gpuCheckParticleFlag)
	{
		ImGui::SliderInt("EnemyIndex", &meshIndex, 0, MESH_MAX_NUM - 1);
		KazImGuiHelper::InputTransform3D("Mother", &model.data.transform);
		if (meshIndex != prevMeshIndex)
		{
			model.data.handle = FbxModelResourceMgr::Instance()->LoadModel(filePass[meshIndex]);
			prevMeshIndex = meshIndex;
		}
		ImGui::SliderInt("FlameAlpha", &model.data.colorData.color.a, 0, 255);
		ImGui::Checkbox("Dead", &deadParticleFlag);
		if (deadParticleFlag != prevDeadParticleFlag)
		{
			deadParticle.reset();
			//deadParticle = std::make_unique<DeadParticle>(meshEmitter[meshIndex]->GetAddress(), meshEmitter[meshIndex]->GetVertNum());

			KazMath::Transform3D lTrans;
			texMotherMat = lTrans.GetMat();
			deadParticle->Init(&texMotherMat);
			prevDeadParticleFlag = deadParticleFlag;
		}

	}
	else if (perlinNoizeFlag)
	{
	}
	else if (textureParticleFlag)
	{
		KazImGuiHelper::InputTransform3D("Mother", &texTransform);
		texMotherMat = texTransform.GetMat();
	}
	else if (splineParticleFlag)
	{
		if (ImGui::TreeNode("ControlPoint1"))
		{
			KazImGuiHelper::InputVec3("ControlPointPos1", &limitPos[0]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ControlPoint2"))
		{
			KazImGuiHelper::InputVec3("ControlPointPos2", &limitPos[1]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ControlPoint3"))
		{
			KazImGuiHelper::InputVec3("ControlPointPos3", &limitPos[2]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ControlPoint4"))
		{
			KazImGuiHelper::InputVec3("ControlPointPos4", &limitPos[3]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ControlPoint5"))
		{
			KazImGuiHelper::InputVec3("ControlPointPos5", &limitPos[4]);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("ControlPoint6"))
		{
			KazImGuiHelper::InputVec3("ControlPointPos6", &limitPos[5]);
			ImGui::TreePop();
		}
		ImGui::Checkbox("Init", &initSplineFlag);
	}
	else if (cpuCheckTriangleAndSphereHitBoxFlag)
	{
		if (ImGui::TreeNode("TrianglePosArray"))
		{
			KazImGuiHelper::InputVec3("Pos1", &hitBoxTrianglePosArray[0]);
			KazImGuiHelper::InputVec3("Pos2", &hitBoxTrianglePosArray[1]);
			KazImGuiHelper::InputVec3("Pos3", &hitBoxTrianglePosArray[2]);
			ImGui::TreePop();
		}
		KazImGuiHelper::InputVec3("PointPos", &pointPos);
		ImGui::DragFloat("PointRadius", &pointRadius);
	}
	else if (gpuCheckHitBoxFlag)
	{
		KazImGuiHelper::InputTransform3D("MeshTransform", &meshTransform);
	}
	else if (cpuCheckMeshParticleAndSphereHitBoxFlag)
	{
		initParticleFlag = ImGui::Button("InitParticle");
		ImGui::Checkbox("DrawParticle", &drawParticleFlag);
		KazImGuiHelper::InputVec3("MeshPos", &meshPos);
		KazImGuiHelper::InputVec3("Mesh2Pos", &mesh2Pos);
		KazImGuiHelper::InputVec3("LinePos1", &vec1->data.startPos);
		KazImGuiHelper::InputVec3("LinePos2", &vec2->data.endPos);
		ImGui::Text("Angle%f", angleParticle);
	}
	else if (drawMeshHitBoxFlag)
	{
		KazImGuiHelper::InputVec3("CPUMeshPos", &collisionPos);
		KazImGuiHelper::InputVec3("CPUMeshPos2", &collisionPos2);
		KazImGuiHelper::InputVec3("CPUMeshPos3", &collisionPos3);
		KazImGuiHelper::InputVec3("MeshCollisonPos1", &meshCollisionPos[0]);
		KazImGuiHelper::InputVec3("MeshCollsionPos2", &meshCollisionPos[1]);
		ImGui::DragInt("LightX1", &color1.lightData.x);
		ImGui::DragInt("LightY1", &color1.lightData.y);
		ImGui::DragInt("LightX2", &color2.lightData.x);
		ImGui::DragInt("LightY2", &color2.lightData.y);
	}
	ImGui::End();


	if (cpuCheckParticleFlag)
	{
		triangelLine[0].data.startPos = triangelPosArray[0];
		triangelLine[0].data.endPos = triangelPosArray[1];
		triangelLine[1].data.startPos = triangelPosArray[1];
		triangelLine[1].data.endPos = triangelPosArray[2];
		triangelLine[2].data.startPos = triangelPosArray[2];
		triangelLine[2].data.endPos = triangelPosArray[0];

		KazMath::Vec3<float>lCentralPos = {};
		//法線ベクトルの算出
		for (int i = 0; i < triangelPosArray.size(); ++i)
		{
			lCentralPos += triangelPosArray[i];

			clossTriangelLine[i].data.startPos = triangelLine[i].data.startPos + (triangelLine[i].data.endPos - triangelLine[i].data.startPos) / 2.0f;
			clossTriangelLine[i].data.colorData.color = { 255,0,0,255 };
		}
		KazMath::Vec3<float>lTriangleCentralPos = lCentralPos / 3.0f;
		for (int i = 0; i < clossTriangelLine.size(); ++i)
		{
			clossTriangelLine[i].data.endPos = lTriangleCentralPos;
		}


		//パーティクルの計算処理
		if (initFlag)
		{
			int particleNum = 0;

			float triangleArea = CalucurateTriangleArea(triangelLine[0].data.startPos, triangelLine[1].data.startPos, triangelLine[2].data.startPos);


			//一つのレイ当たりで使用できるパーティクルを決定する
			const int L_MAX_PARTICLE = static_cast<int>(particle.size()) / static_cast<int>(triangelPosArray.size());
			for (int rayIndex = 0; rayIndex < triangelLine.size(); ++rayIndex)
			{
				KazMath::Vec3<float>lDistance = triangelLine[rayIndex].data.endPos - triangelLine[rayIndex].data.startPos;

				//以下の項目を計算し、配置する
				for (int i = 0; i < L_MAX_PARTICLE; ++i)
				{
					//1.レイに沿った座標を乱数で決定する
					float lRate = KazMath::Rand(1.0f, 0.0f);
					KazMath::Vec3<float>lStartPos = triangelLine[rayIndex].data.startPos + lDistance * lRate;

					//2.1で決めた座標を始点、重心座標を終点とし、その長さを求める
					KazMath::Vec3<float>lResultDistance = lTriangleCentralPos - lStartPos;


					//数が多いほどエッジ周辺に偏らせる
					const int L_RANDOM_NUMBER_BIAS = particleBias;

					//3.先程決めた長さの内、エッジ周辺にパーティクルを配置するか、長さの範囲で座標を決めるか乱数で決める
					if (KazMath::Rand(PARTICLE_MAX_BIAS, 0) <= L_RANDOM_NUMBER_BIAS)
					{
						//4-1.エッジ周辺にパーティクルを配置する場合は、長さを一定値割った値から乱数でどの場所に配置するか決める
						particle[particleNum].data.transform.pos = lStartPos + lResultDistance * KazMath::Rand(0.1f, 0.0f);
					}
					else
					{
						//4-2.長さの範囲で座標を決める場合は長さの値でどの場所に配置するか決める
						particle[particleNum].data.transform.pos = lStartPos + lResultDistance * KazMath::Rand(1.0f, 0.0f);
					}

					//UV--------------------
					float u = CalucurateUVW(triangelLine[0].data.startPos, triangelLine[0].data.endPos, particle[particleNum].data.transform.pos, triangleArea);
					float v = CalucurateUVW(triangelLine[1].data.startPos, triangelLine[1].data.endPos, particle[particleNum].data.transform.pos, triangleArea);
					float w = CalucurateUVW(triangelLine[2].data.startPos, triangelLine[2].data.endPos, particle[particleNum].data.transform.pos, triangleArea);

					KazMath::Vec3<float> uvw;
					KazMath::Vec2<float> uVec = KazMath::Vec2<float>(0.0f, 0.0f) * u;
					KazMath::Vec2<float> vVec = KazMath::Vec2<float>(0.0f, 1.0f) * v;
					KazMath::Vec2<float> wVec = KazMath::Vec2<float>(1.0f, 0.0f) * w;
					//UV--------------------


					particle[particleNum].data.transform.scale = { 0.1f,0.1f,0.1f };
					++particleNum;
				}
			}

			for (int i = particleNum; i < particle.size(); ++i)
			{
				particle[i].data.transform.scale = { 0.01f,0.01f,0.01f };
			}

			initFlag = false;
		}
	}
	else if (gpuCheckParticleFlag)
	{
		motherMat = model.motherMat;

		if (!deadParticleFlag)
		{
			meshEmitter[meshIndex]->Update();
		}
		else
		{
			deadParticle->Update();
		}

	}
	else if (perlinNoizeFlag)
	{
	}
	else if (textureParticleFlag)
	{
		texParticle->Update();
	}
	else if (splineParticleFlag)
	{
		for (int i = 0; i < controlPointR.size(); ++i)
		{
			controlPointR[i].data.transform.pos = limitPos[i];
			controlPointR[i].data.transform.scale = { 20.0f,20.0f,20.0f };
		}
		if (initSplineFlag)
		{
			splineParticle->Init(limitPos, false);
		}
		splineParticle->Update();
	}
	else if (cpuCheckTriangleAndSphereHitBoxFlag)
	{
		KazMath::Vec3<float>lClosestPos = ClosestPoint(pointPos, hitBoxTrianglePosArray[0], hitBoxTrianglePosArray[1], hitBoxTrianglePosArray[2]);

		//当たり判定
		if (lClosestPos.Distance(pointPos) <= pointRadius)
		{
			pointPosR.data.color = { 255,0,0,255 };
		}
		else
		{
			pointPosR.data.color = { 255,255,255,255 };
		}
		hitBoxTriangelLine[0].data.startPos = hitBoxTrianglePosArray[0];
		hitBoxTriangelLine[0].data.endPos = hitBoxTrianglePosArray[1];
		hitBoxTriangelLine[1].data.startPos = hitBoxTrianglePosArray[1];
		hitBoxTriangelLine[1].data.endPos = hitBoxTrianglePosArray[2];
		hitBoxTriangelLine[2].data.startPos = hitBoxTrianglePosArray[2];
		hitBoxTriangelLine[2].data.endPos = hitBoxTrianglePosArray[0];

		pointPosR.data.transform.pos = pointPos;
		pointPosR.data.transform.scale = { pointRadius,pointRadius,pointRadius };

		closestPointPosR.data.transform.pos = lClosestPos;
	}
	else if (gpuCheckHitBoxFlag)
	{
		meshMat = meshTransform.GetMat();
		collision->Update(particleWall->GetBasePosHandle(), particleWall->GetParticleHandle(), meshTransform.Dirty());
		meshTransform.Record();
		particleWall->Update();
	}
	else if (cpuCheckMeshParticleAndSphereHitBoxFlag)
	{

		vec1->data.endPos = { 0.0f,0.0f,0.0f };
		vec2->data.startPos = { 0.0f,0.0f,0.0f };

		KazMath::Vec3<float>vec1Dir = vec1->data.endPos - vec1->data.startPos;
		KazMath::Vec3<float>vec2Dir = vec2->data.endPos - vec2->data.startPos;

		float lTCosf = vec1Dir.Dot(vec2Dir) / (vec1Dir.Length() * vec2Dir.Length());
		float lTSinf = acos(lTCosf);

		angleParticle = static_cast<float>(KazMath::RadianToAngle(lTSinf));

		//座標初期化
		if (initParticleFlag)
		{
			blockPos = { -10.0f,0.0f,0.0f };
			blockPos2 = { -12.0f,0.0f,0.0f };
			InitMeshParticle(blockPos, particleHitBox, baseParticleHitBox);
			InitMeshParticle(blockPos2, particleHitBox2, baseParticleHitBox2);

			for (int surfaceIndex = 0; surfaceIndex < baseParticleHitBox.size(); ++surfaceIndex)
			{
				for (int i = 0; i < baseParticleHitBox[surfaceIndex].size(); ++i)
				{
					//particleLarpPos[surfaceIndex][i] = baseParticleHitBox[surfaceIndex][i]->data.transform.pos;
				}
			}

			for (int surfaceIndex = 0; surfaceIndex < baseParticleHitBox2.size(); ++surfaceIndex)
			{
				for (int i = 0; i < baseParticleHitBox2[surfaceIndex].size(); ++i)
				{
					//particleLarpPos2[surfaceIndex][i] = baseParticleHitBox2[surfaceIndex][i]->data.transform.pos;
				}
			}
		}

		//当たり判定
		//if (meshPos != prevMeshPos)
		{
			Sphere lSphere1, lMeshSphere;
			lMeshSphere.center = &meshPos;
			lMeshSphere.radius = 3.0f;

			lSphere1.center = &blockPos;
			lSphere1.radius = 5.0f;

			Sphere lSphere2;
			lSphere2.center = &blockPos2;
			lSphere2.radius = 5.0f;


			bool lHitFlag = CollisionManager::Instance()->CheckSphereAndSphere(lMeshSphere, lSphere1) || CollisionManager::Instance()->CheckSphereAndSphere(lMeshSphere, lSphere2);
			if (lHitFlag)
			{
				//対象判定一つ目
				CollisionDetection(particleHitBox, particleLarpPos, meshPos, blockPos);
				CollisionDetection(particleHitBox2, particleLarpPos2, meshPos, blockPos2);
			}
			else
			{
				for (int surfaceIndex = 0; surfaceIndex < particleLarpPos.size(); ++surfaceIndex)
				{
					for (int i = 0; i < particleLarpPos[surfaceIndex].size(); ++i)
					{
						particleLarpPos[surfaceIndex][i] = {};
						particleLarpPos2[surfaceIndex][i] = {};
					}
				}
			}
			lMeshSphere.center = &mesh2Pos;
			lMeshSphere.radius = 3.0f;
			bool lHitFlag2 = CollisionManager::Instance()->CheckSphereAndSphere(lMeshSphere, lSphere1) || CollisionManager::Instance()->CheckSphereAndSphere(lMeshSphere, lSphere2);
			if (lHitFlag2)
			{
				//対象判定二つ目
				CollisionDetection(particleHitBox, particleLarpPos3, mesh2Pos, blockPos);
				CollisionDetection(particleHitBox2, particleLarpPos4, mesh2Pos, blockPos2);
			}
			else
			{
				for (int surfaceIndex = 0; surfaceIndex < particleLarpPos3.size(); ++surfaceIndex)
				{
					for (int i = 0; i < particleLarpPos3[surfaceIndex].size(); ++i)
					{
						particleLarpPos3[surfaceIndex][i] = {};
						particleLarpPos4[surfaceIndex][i] = {};
					}
				}
			}



			if (lHitFlag || lHitFlag2)
			{
				larpVel = 0.5f;

				for (int surfaceIndex = 0; surfaceIndex < particleHitBox.size(); ++surfaceIndex)
				{
					for (int i = 0; i < particleHitBox[surfaceIndex].size(); ++i)
					{
						KazMath::Vec3<float>lVel = particleLarpPos[surfaceIndex][i] + particleLarpPos3[surfaceIndex][i];

						blockParticleLarpPos[surfaceIndex][i] = baseParticleHitBox[surfaceIndex][i]->data.transform.pos + lVel;
					}
				}
				for (int surfaceIndex = 0; surfaceIndex < particleHitBox2.size(); ++surfaceIndex)
				{
					for (int i = 0; i < particleHitBox2[surfaceIndex].size(); ++i)
					{
						KazMath::Vec3<float>lVel = particleLarpPos2[surfaceIndex][i] + particleLarpPos4[surfaceIndex][i];

						blockParticleLarpPos2[surfaceIndex][i] = baseParticleHitBox2[surfaceIndex][i]->data.transform.pos + lVel;
					}
				}
			}
			else if (!lHitFlag && !lHitFlag2)
			{
				larpVel = 0.05f;
				for (int surfaceIndex = 0; surfaceIndex < particleHitBox.size(); ++surfaceIndex)
				{
					for (int i = 0; i < particleHitBox[surfaceIndex].size(); ++i)
					{
						blockParticleLarpPos[surfaceIndex][i] = baseParticleHitBox[surfaceIndex][i]->data.transform.pos;
					}
				}
				for (int surfaceIndex = 0; surfaceIndex < particleHitBox2.size(); ++surfaceIndex)
				{
					for (int i = 0; i < particleHitBox2[surfaceIndex].size(); ++i)
					{
						blockParticleLarpPos2[surfaceIndex][i] = baseParticleHitBox2[surfaceIndex][i]->data.transform.pos;
					}
				}
			}
			prevMeshPos = meshPos;
		}

		circleModelR.data.transform.pos = meshPos;
		circleModelR.data.transform.scale = { 2.0f,2.0f,2.0f };
		circleModelR2.data.transform.pos = mesh2Pos;
		circleModelR2.data.transform.scale = { 2.0f,2.0f,2.0f };
	}
	else if (drawInstanceMeshParticleFlag)
	{
		particleRender->InitCount();
		meshParticle->Compute();
	}
	else if (drawMeshHitBoxFlag)
	{
		particleRender->InitCount();

		meshCollision->Compute();


		modelHitBox->data.transform.pos = collisionPos;
		modelHitBox->data.transform.scale = { 1.0f,1.0f,1.0f };

		for (int i = 0; i < meshCollisionPos.size(); ++i)
		{
			meshCollisionMat[i] = KazMath::CaluWorld(meshCollisionPos[i], { 0.0f,1.0f,0.0f }, { 0.0f,0.0f,1.0f });
		}
	}
}

void DebugMeshParticleScene::Draw()
{
	RenderTargetStatus::Instance()->SetDoubleBufferFlame();
	RenderTargetStatus::Instance()->ClearDoubuleBuffer({ 0.0f,0.0f,150.0f });

	rendertarget->SetRenderTarget();

	if (cpuCheckParticleFlag)
	{
		//三角形描画
		for (int i = 0; i < triangelLine.size(); ++i)
		{
			triangelLine[i].Draw();
		}

		for (int i = 0; i < drawParticleNum; ++i)
		{
			particle[i].Draw();
		}
		for (int i = 0; i < clossTriangelLine.size(); ++i)
		{
			clossTriangelLine[i].Draw();
		}
	}
	else if (gpuCheckParticleFlag)
	{
		if (!deadParticleFlag)
		{
			meshEmitter[meshIndex]->Draw();
		}
		else
		{
			deadParticle->Draw();
		}
		model.Draw();
	}
	else if (perlinNoizeFlag)
	{
	}
	else if (textureParticleFlag)
	{
		texParticle->Draw();
	}
	else if (splineParticleFlag)
	{
		splineParticle->Draw();

		for (int i = 0; i < controlPointR.size(); ++i)
		{
			controlPointR[i].Draw();
		}
	}
	else if (cpuCheckTriangleAndSphereHitBoxFlag)
	{
		for (int i = 0; i < hitBoxTriangelLine.size(); ++i)
		{
			hitBoxTriangelLine[i].Draw();
		}

		pointPosR.Draw();
		closestPointPosR.Draw();
	}
	else if (gpuCheckHitBoxFlag)
	{
		//collision->Draw();
		particleWall->Draw();
	}
	else if (cpuCheckMeshParticleAndSphereHitBoxFlag)
	{
		if (drawParticleFlag)
		{
			for (int surfaceIndex = 0; surfaceIndex < particleHitBox.size(); ++surfaceIndex)
			{
				for (int i = 0; i < particleHitBox[surfaceIndex].size(); ++i)
				{
					KazMath::Larp(blockParticleLarpPos[surfaceIndex][i], &particleHitBox[surfaceIndex][i]->data.transform.pos, larpVel);

					particleHitBox[surfaceIndex][i]->Draw();
				}
			}
			for (int surfaceIndex = 0; surfaceIndex < particleHitBox2.size(); ++surfaceIndex)
			{
				for (int i = 0; i < particleHitBox2[surfaceIndex].size(); ++i)
				{
					KazMath::Larp(blockParticleLarpPos2[surfaceIndex][i], &particleHitBox2[surfaceIndex][i]->data.transform.pos, larpVel);

					particleHitBox2[surfaceIndex][i]->Draw();
				}
			}
			circleModelR.Draw();
			circleModelR2.Draw();
		}
		else
		{
			vec1->Draw();
			vec2->Draw();
		}
	}
	else if (drawInstanceMeshParticleFlag)
	{
		particleRender->Draw();
	}
	else if (drawMeshHitBoxFlag)
	{
		modelHitBox->Draw();
		particleRender->Draw();
	}
	if (drawGridFlag)
	{
		//debug.Draw();
	}



	rendertarget->Draw();
	//debugDraw.Draw();
	mainRenderTarget.Draw();
}

int DebugMeshParticleScene::SceneChange()
{
	if (KeyBoradInputManager::Instance()->InputTrigger(DIK_0))
	{
		return 0;
	}
	else
	{
		return SCENE_NONE;
	}
}

float DebugMeshParticleScene::PerlinNoize(const KazMath::Vec2<float> &UV, int SEED)
{
	KazMath::Vec2<float> p = { std::floor(UV.x),std::floor(UV.y) };
	KazMath::Vec2<float> f = { Frac(UV.x),Frac(UV.y) };

	int lS = SEED;
	lS = 0;

	KazMath::Vec2<float>dot1(KazMath::Rand(1.0f, -1.0f), KazMath::Rand(1.0f, -1.0f));
	float w00 = dot1.Dot(f);

	KazMath::Vec2<float>dot2(KazMath::Rand(1.0f, -1.0f), KazMath::Rand(1.0f, -1.0f));
	float w10 = dot1.Dot(f);

	KazMath::Vec2<float>dot3(KazMath::Rand(1.0f, -1.0f), KazMath::Rand(1.0f, -1.0f));
	float w01 = dot1.Dot(f);

	KazMath::Vec2<float>dot4(KazMath::Rand(1.0f, -1.0f), KazMath::Rand(1.0f, -1.0f));
	float w11 = dot1.Dot(f);

	KazMath::Vec2<float> u = f * f * (KazMath::Vec2<float>(3.0f, 3.0f) - (f * 2.0f));


	float l1 = Larp(w00, w10, u.x);
	float l2 = Larp(w01, w11, u.x);

	return Larp(l1, l2, u.y);
}

KazMath::Vec3<float> DebugMeshParticleScene::ClosestPoint(KazMath::Vec3<float> POINT_POS, KazMath::Vec3<float> TRIANGLE_A_POS, KazMath::Vec3<float> TRIANGLE_B_POS, KazMath::Vec3<float> TRIANGLE_C_POS)
{
	//点Pが三角形Aの外側の頂点領域の中にあるか確認--------------------------------------------

	KazMath::Vec3<float> lAb = TRIANGLE_B_POS - TRIANGLE_A_POS;
	KazMath::Vec3<float> lAc = TRIANGLE_C_POS - TRIANGLE_A_POS;
	KazMath::Vec3<float> lAp = POINT_POS - TRIANGLE_A_POS;

	float lD1 = lAb.Dot(lAp);
	float lD2 = lAc.Dot(lAp);

	if (lD1 <= 0.0f && lD2 <= 0.0f)
	{
		return TRIANGLE_A_POS;
	}

	//点Pが三角形Aの外側の頂点領域の中にあるか確認--------------------------------------------



	//点Pが三角形Bの外側の頂点領域の中にあるか確認--------------------------------------------

	KazMath::Vec3<float> lBp = POINT_POS - TRIANGLE_B_POS;
	float lD3 = lAb.Dot(lBp);
	float lD4 = lAc.Dot(lBp);
	if (0.0f <= lD3 && lD4 <= lD3)
	{
		return TRIANGLE_B_POS;
	}

	//点Pが三角形Bの外側の頂点領域の中にあるか確認--------------------------------------------


	//点PがABの辺領域の中にあるかどうか確認し、あればPのAB上に対する射影を返す--------------------------------------------

	float lVc = lD1 * lD4 - lD3 * lD2;
	if (lVc <= 0.0f && 0.0f <= lD1 && lD3 <= 0.0f)
	{
		float lV = lD1 / (lD1 - lD3);
		return TRIANGLE_A_POS + lAb * lV;
	}

	//点PがABの辺領域の中にあるかどうか確認し、あればPのAB上に対する射影を返す--------------------------------------------

	//点PがCの外側の頂点領域の中にあるかどうか確認--------------------------------------------

	KazMath::Vec3<float>lCp = POINT_POS - TRIANGLE_C_POS;
	float lD5 = lAb.Dot(lCp);
	float lD6 = lAc.Dot(lCp);
	if (0.0f <= lD6 && lD5 <= lD6)
	{
		return TRIANGLE_C_POS;
	}
	//点PがCの外側の頂点領域の中にあるかどうか確認--------------------------------------------



	//点PがACの辺領域の中にあるかどうか確認し、あればPのAC上に対する射影を返す--------------------------------------------

	float lVb = lD5 * lD2 - lD1 * lD6;
	if (lVb <= 0.0f && 0.0f <= lD2 && lD6 <= 0.0f)
	{
		float lW = lD2 / (lD2 - lD6);
		return TRIANGLE_A_POS + lAc * lW;
	}

	//点PがACの辺領域の中にあるかどうか確認し、あればPのAC上に対する射影を返す--------------------------------------------


	//点PがBCの辺領域の中にあるかどうか確認し、あればPのBC上に対する射影を返す--------------------------------------------

	float lVa = lD3 * lD6 - lD5 * lD4;
	float lD4SubD3 = lD4 - lD3;
	float lD5SubD6 = lD5 - lD6;
	if (lVa <= 0.0f && 0.0f <= lD4SubD3 && 0.0f <= lD5SubD6)
	{
		float lW = lD4SubD3 / (lD4SubD3 + lD5SubD6);
		return TRIANGLE_B_POS + (TRIANGLE_C_POS - TRIANGLE_B_POS) * lW;
	}

	//点PがBCの辺領域の中にあるかどうか確認し、あればPのBC上に対する射影を返す--------------------------------------------


	//点Pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算--------------------------------------------
	float lDenom = 1.0f / (lVa + lVb + lVc);
	float lV = lVb * lDenom;
	float lW = lVc * lDenom;
	return TRIANGLE_A_POS + lAb * lV + lAc * lW;;
}

void DebugMeshParticleScene::InitMeshParticle(const KazMath::Vec3<float> &BASE_POS, std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>> &PARTICLE_DATA, std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>> &BASE_PARTICLE_DATA)
{
	std::array<KazMath::Vec3<float>, 36>lVertices = GetSquareVertData(BASE_POS);

	const unsigned short lIndices[] =
	{
		0,1,2,
		2,1,3,

		4,6,5,
		6,7,5,

		8,9,10,
		10,9,11,

		12,14,13,
		13,14,15,

		16,18,17,
		17,18,19,

		20,21,22,
		22,21,23
	};

	for (int i = 0; i < PARTICLE_DATA.size(); ++i)
	{
		std::array<TriangleLineData, 3> lLineDataArray;
		int lIndicesIndex = 0;
		int lIndexOffset = 0;//0-1,1-2,2-0と言った順に三角形を繋ぐためのオフセット

		for (int lineIndex = 0; lineIndex < 3; ++lineIndex)
		{
			lIndicesIndex = i * 3 + lIndexOffset;
			lLineDataArray[lineIndex].startPos = lVertices[lIndices[lIndicesIndex]];

			//インデックスずらし
			++lIndexOffset;
			if (3 <= lIndexOffset)
			{
				lIndexOffset = 0;
			}

			//終点設定
			lIndicesIndex = i * 3 + lIndexOffset;
			lLineDataArray[lineIndex].endPos = lVertices[lIndices[lIndicesIndex]];
		}

		SetParticle(lLineDataArray, PARTICLE_DATA[i], BASE_PARTICLE_DATA[i]);
	}
}

void DebugMeshParticleScene::SetParticle(std::array<TriangleLineData, 3>LINE_ARRAY_POS, std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM> &PARTICLE_DATA, std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM> &BASE_PARTICLE_DATA)
{
	int lParticleNum = 0;
	const float L_SCALE = 0.01f;

	KazMath::Vec3<float>lTriangleCentralPos = (LINE_ARRAY_POS[0].startPos + LINE_ARRAY_POS[1].startPos + LINE_ARRAY_POS[2].startPos) / 3.0f;
	float lTriangleArea = CalucurateTriangleArea(LINE_ARRAY_POS[0].startPos, LINE_ARRAY_POS[1].startPos, LINE_ARRAY_POS[2].startPos);

	//一つのレイ当たりで使用できるパーティクルを決定する
	const int L_MAX_PARTICLE = static_cast<int>(PARTICLE_DATA.size()) / static_cast<int>(LINE_ARRAY_POS.size());
	for (int rayIndex = 0; rayIndex < LINE_ARRAY_POS.size(); ++rayIndex)
	{
		KazMath::Vec3<float>lDistance = LINE_ARRAY_POS[rayIndex].endPos - LINE_ARRAY_POS[rayIndex].startPos;

		//以下の項目を計算し、配置する
		for (int i = 0; i < L_MAX_PARTICLE; ++i)
		{
			PARTICLE_DATA[lParticleNum].reset();

			//1.レイに沿った座標を乱数で決定する
			float lRate = KazMath::Rand(1.0f, 0.0f);
			KazMath::Vec3<float>lStartPos = LINE_ARRAY_POS[rayIndex].startPos + lDistance * lRate;

			//2.1で決めた座標を始点、重心座標を終点とし、その長さを求める
			KazMath::Vec3<float>lResultDistance = lTriangleCentralPos - lStartPos;


			//数が多いほどエッジ周辺に偏らせる
			const int L_RANDOM_NUMBER_BIAS = 100;

			PARTICLE_DATA[lParticleNum] = std::make_unique<BoxPolygonRender>();

			//3.先程決めた長さの内、エッジ周辺にパーティクルを配置するか、長さの範囲で座標を決めるか乱数で決める
			if (KazMath::Rand(PARTICLE_MAX_BIAS, 0) <= L_RANDOM_NUMBER_BIAS)
			{
				//4-1.エッジ周辺にパーティクルを配置する場合は、長さを一定値割った値から乱数でどの場所に配置するか決める
				PARTICLE_DATA[lParticleNum]->data.transform.pos = lStartPos + lResultDistance * KazMath::Rand(0.1f, 0.0f);
			}
			else
			{
				//4-2.長さの範囲で座標を決める場合は長さの値でどの場所に配置するか決める
				PARTICLE_DATA[lParticleNum]->data.transform.pos = lStartPos + lResultDistance * KazMath::Rand(1.0f, 0.0f);
			}

			//UV--------------------
			float u = CalucurateUVW(LINE_ARRAY_POS[0].startPos, LINE_ARRAY_POS[0].endPos, PARTICLE_DATA[lParticleNum]->data.transform.pos, lTriangleArea);
			float v = CalucurateUVW(LINE_ARRAY_POS[1].startPos, LINE_ARRAY_POS[1].endPos, PARTICLE_DATA[lParticleNum]->data.transform.pos, lTriangleArea);
			float w = CalucurateUVW(LINE_ARRAY_POS[2].startPos, LINE_ARRAY_POS[2].endPos, PARTICLE_DATA[lParticleNum]->data.transform.pos, lTriangleArea);

			KazMath::Vec3<float> uvw;
			KazMath::Vec2<float> uVec = KazMath::Vec2<float>(0.0f, 0.0f) * u;
			KazMath::Vec2<float> vVec = KazMath::Vec2<float>(0.0f, 1.0f) * v;
			KazMath::Vec2<float> wVec = KazMath::Vec2<float>(1.0f, 0.0f) * w;
			//UV--------------------


			PARTICLE_DATA[lParticleNum]->data.transform.scale = { L_SCALE,L_SCALE,L_SCALE };


			BASE_PARTICLE_DATA[lParticleNum].reset();
			BASE_PARTICLE_DATA[lParticleNum] = std::make_unique<BoxPolygonRender>();
			BASE_PARTICLE_DATA[lParticleNum]->data.transform = PARTICLE_DATA[lParticleNum]->data.transform;
			++lParticleNum;
		}
	}

	for (int i = lParticleNum; i < PARTICLE_DATA.size(); ++i)
	{
		PARTICLE_DATA[i].reset();
		PARTICLE_DATA[i] = std::make_unique<BoxPolygonRender>();
		PARTICLE_DATA[i]->data.transform.scale = { L_SCALE,L_SCALE,L_SCALE };

		BASE_PARTICLE_DATA[i].reset();
		BASE_PARTICLE_DATA[i] = std::make_unique<BoxPolygonRender>();
		BASE_PARTICLE_DATA[i]->data.transform = PARTICLE_DATA[i]->data.transform;
	}
}

void DebugMeshParticleScene::CollisionDetection(std::vector<std::array<BoxPolygonRenderPtr, PARTICLE_HITBOX_NUM>> &PARTICLE_DATA, std::array<std::array<KazMath::Vec3<float>, PARTICLE_HITBOX_NUM>, 12> &LARP_POS, const KazMath::Vec3<float> &MESH_POS, const KazMath::Vec3<float> &BLOCK_POS)
{
	//ブロックの中心座標と対象判定の中心座標の方向を見る
	KazMath::Vec3<float> lCentralToBlockDir = MESH_POS - BLOCK_POS;

	for (int surfaceIndex = 0; surfaceIndex < PARTICLE_DATA.size(); ++surfaceIndex)
	{
		for (int i = 0; i < PARTICLE_DATA[surfaceIndex].size(); ++i)
		{
			//パーティクルの当たり判定計算用の座標
			KazMath::Vec3<float> lHitBoxPos = PARTICLE_DATA[surfaceIndex][i]->data.transform.pos - BLOCK_POS;

			//パーティクル座標と対象判定座標の方向を見る
			KazMath::Vec3<float> lCentralToParticleDir = lCentralToBlockDir - lHitBoxPos;

			//なす角の判断
			float lCosf = lCentralToParticleDir.Dot(lCentralToBlockDir) / (lCentralToParticleDir.Length() * lCentralToBlockDir.Length());
			float lSinf = acos(lCosf);

			//移動量算出
			int lAngle = KazMath::RadianToAngle(lSinf) - 90;
			float rate = static_cast<float>(lAngle) / 90.0f;

			lCentralToParticleDir.Normalize();
			lCentralToParticleDir.x = 0.0f;
			LARP_POS[surfaceIndex][i] = lCentralToParticleDir * (5.5f * rate);

			particleAngle[surfaceIndex][i] = lAngle;
		}
	}
}

std::array<KazMath::Vec3<float>, 36> DebugMeshParticleScene::GetSquareVertData(const KazMath::Vec3<float> &BASE_POS)
{
	const float lSize = 1.0f;
	Vertex lVertices[] =
	{
	{{-lSize, -lSize, -lSize},{}, {0.0f,1.0f}},
	{{-lSize,  lSize, -lSize},{}, {0.0f,0.0f}},
	{{ lSize, -lSize, -lSize}, {}, {1.0f,1.0f}},
	{{ lSize,  lSize, -lSize}, {}, {1.0f,0.0f}},

	{{-lSize,-lSize,  lSize}, {}, {0.0f,1.0f}},
	{{-lSize, lSize,  lSize}, {},{0.0f,0.0f}},
	{{ lSize,-lSize,  lSize},{},{1.0f,1.0f}},
	{{ lSize, lSize,  lSize},{}, {1.0f,0.0f}},

	{{-lSize,-lSize, -lSize},{}, {0.0f,1.0f}},
	{{-lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},
	{{-lSize, lSize, -lSize}, {}, {1.0f,1.0f}},
	{{-lSize, lSize,  lSize}, {}, {1.0f,0.0f}},

	{{lSize,-lSize, -lSize},{}, {0.0f,1.0f}},
	{{lSize,-lSize,  lSize}, {}, {0.0f,0.0f}},
	{{lSize, lSize, -lSize}, {}, {1.0f,1.0f}},
	{{lSize, lSize,  lSize}, {}, {1.0f,0.0f}},
	{{ lSize, -lSize, lSize}, {}, {0.0f,1.0f}},
	{{ lSize, -lSize,-lSize}, {}, {0.0f,0.0f}},
	{{-lSize, -lSize, lSize}, {}, {1.0f,1.0f}},
	{{-lSize, -lSize,-lSize}, {}, {1.0f,0.0f}},
	{{ lSize, lSize, lSize}, {}, {0.0f,1.0f}},
	{{ lSize, lSize,-lSize}, {}, {0.0f,0.0f}},
	{{-lSize, lSize, lSize}, {}, {1.0f,1.0f}},
	{{-lSize, lSize,-lSize}, {}, {1.0f,0.0f}}
	};

	std::array<KazMath::Vec3<float>, 36>lPosArray;
	for (int i = 0; i < 36; ++i)
	{
		lPosArray[i].x = BASE_POS.x + lVertices[i].pos.x;
		lPosArray[i].y = BASE_POS.y + lVertices[i].pos.y;
		lPosArray[i].z = BASE_POS.z + lVertices[i].pos.z;
	}

	return lPosArray;
}
