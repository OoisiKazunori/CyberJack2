#include "DemoScene.h"
#include"../Game/Input/Input.h"

DemoScene::DemoScene() :
	//DrawFuncHelperでのテクスチャ読み込み
	m_2DSprite("Resource/Test/texas.png"),
	m_3DSprite("Resource/Test/texas.png"),
	//DrawFuncHelperでのモデル読み込み
	m_modelAnimationRender("Resource/Test/Virus/", "virus_cur.gltf"),
	m_modelRender("Resource/Test/Virus/", "virus_cur.gltf")
{

	/*
	テクスチャやモデルの読み込みはTextureRenderやModelRenderのコンストラクタで読み込まれますが、
	読み込み単体の処理は下の処理になります。(多重読み込み防止あり)
	読み込みの成否はコンソールに出力されます
	*/
	std::shared_ptr<ModelInfomation> model = ModelLoader::Instance()->Load("Resource/Test/Virus/", "virus_cur.gltf");
	KazBufferHelper::BufferData texture = TextureResourceMgr::Instance()->LoadGraphBuffer("Resource/Test/texas.png");

	//アニメーション再生
	m_modelAnimationRender.m_animator->Play("繧｢繝ｼ繝槭メ繝･繧｢Action", true);

	//音声読み込み
	m_bgmHandle = SoundManager::Instance()->SoundLoadWave("Resource/Sound/bgm.wav");
	m_seHandle = SoundManager::Instance()->SoundLoadWave("Resource/Sound/done.wav");
	m_seHandle.volume = 0.005f;

	//音声再生
	m_bgmHandle.volume = 0.005f;
	SoundManager::Instance()->SoundPlayerWave(m_bgmHandle, 100);

	//グリッドの描画の情報生成--------------------------
	for (int z = 0; z < m_gridCallDataX.size(); ++z)
	{
		std::vector<KazMath::Vec3<float>>posArray(2);
		VertexGenerateData bufferData(posArray.data(), sizeof(DirectX::XMFLOAT3), posArray.size(), sizeof(DirectX::XMFLOAT3));
		m_gridCallDataX[z] = DrawFuncData::SetLine(VertexBufferMgr::Instance()->GenerateBuffer(bufferData, false));
	}
	for (int y = 0; y < m_gridCallDataZ.size(); ++y)
	{
		std::vector<KazMath::Vec3<float>>posArray(2);
		VertexGenerateData bufferData(posArray.data(), sizeof(DirectX::XMFLOAT3), posArray.size(), sizeof(DirectX::XMFLOAT3));
		m_gridCallDataZ[y] = DrawFuncData::SetLine(VertexBufferMgr::Instance()->GenerateBuffer(bufferData, false));
	}

	//3DスプライトのTrasform調整
	m_3DSpriteTransform.pos = { 10.0f,0.0f,0.0f };
	m_3DSpriteTransform.scale = { 0.1f,0.1f,1.0f };
	//アニメーション再生無しモデルの位置調整
	m_modelTransform.pos = { -10.0f,0.0f,0.0f };
}

DemoScene::~DemoScene()
{
}

void DemoScene::Init()
{
}

void DemoScene::PreInit()
{
}

void DemoScene::Finalize()
{
}

void DemoScene::Input()
{
	/*
	コントローラーやキーボードの入力を使った処理を実装する際はInputクラスから使ってください
	新しい操作の処理を実装する際はInputクラスに関数を追加して使用してください(書き換えは自由です)
	(操作変更を容易に行いやすくするためと処理の重複を無くす為にもご協力お願いします。)
	*/
	if (Input::Instance()->Done())
	{
		//音声再生
		SoundManager::Instance()->SoundPlayerWave(m_seHandle, 0);
	}
}

void DemoScene::Update()
{
	/*
	カメラを使用する際は下の関数を使用し、eye, target, upの値を入れることで計算できます
	計算結果は描画情報に渡ります。
	CameraMgr::Instance()->Camera({}, {}, {});
	*/
	//デバック用のカメラワーク(操作はBlenderと同じ)
	m_camera.Update();
}

void DemoScene::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	//描画命令発行
	m_2DSprite.Draw2D(arg_rasterize, m_2DSpriteTransform);
	m_3DSprite.Draw3D(arg_rasterize, m_3DSpriteTransform);
	m_modelAnimationRender.Draw(arg_rasterize, m_modelAnimationTransform);
	m_modelRender.Draw(arg_rasterize, m_modelTransform);

	//下の処理はDrawFuncHelper無しで描画した
	//グリッドの描画--------------------------

	const float height = -5.0f;
	const float range = 50.0f;
	const KazMath::Color lineColor(49, 187, 134, 255);

	//横の線を並べる
	for (int z = 0; z < m_gridCallDataX.size(); ++z)
	{
		float zLine = static_cast<float>(z) * 10.0f - (range);
		KazMath::Vec3<float>startPos(-range, height, zLine), endPos(range, height, zLine);
		std::vector<KazMath::Vec3<float>>posArray;
		posArray.emplace_back(startPos);
		posArray.emplace_back(endPos);
		//DrawLineの描画情報を設定する
		DrawFunc::DrawLine(m_gridCallDataZ[z], posArray, m_gridCallDataZ[z].m_modelVertDataHandle, lineColor);
		//描画命令発行
		arg_rasterize.ObjectRender(m_gridCallDataZ[z]);
	}
	//縦の線を並べる
	for (int x = 0; x < m_gridCallDataZ.size(); ++x)
	{
		float xLine = static_cast<float>(x) * 10.0f - (range);
		KazMath::Vec3<float>startPos(xLine, height, -range), endPos(xLine, height, range);

		std::vector<KazMath::Vec3<float>>posArray;
		posArray.emplace_back(startPos);
		posArray.emplace_back(endPos);
		//DrawLineの描画情報を設定する
		DrawFunc::DrawLine(m_gridCallDataX[x], posArray, m_gridCallDataX[x].m_modelVertDataHandle, lineColor);
		//描画命令発行
		arg_rasterize.ObjectRender(m_gridCallDataX[x]);
	}
}

int DemoScene::SceneChange()
{
	return SCENE_NONE;
}
