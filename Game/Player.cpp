#include "Player.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

const int Player::COOL_MAX_TIME = 120;

Player::Player()
{
	hp = -1;
	pos = {};

	//damageSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "PlayerDamage.wav");

	float lScale = 0.5f;
	minScale = { lScale ,lScale ,lScale };
	sinTimer = 0.0f;

	adjPos = { 0.0f,0.9f,0.3f };


	DrawFuncData::PipelineGenerateData lData;
	lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvLightMain", "vs_6_4", SHADER_TYPE_VERTEX);
	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvLightMain", "ps_6_4", SHADER_TYPE_PIXEL);
	lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
	//m_playerModel = DrawFuncData::SetDrawGLTFIndexMaterialInRayTracingData(*ModelLoader::Instance()->Load("Resource/Test/glTF/Avocado/", "Avocado.gltf"), lData);
	//auto playerModel = *ModelLoader::Instance()->Load("Resource/Player/Kari/", "Player.gltf");
	//auto pipeline = DrawFuncData::GetModelBloomShader();
	//for (auto& index : m_playerModel) {
	//	index = DrawFuncData::SetRaytracingData(playerModel, pipeline);
	//}
}

void Player::Init(const KazMath::Vec3<float>& POS, bool DRAW_UI_FLAG, bool APPEAR_FLAG)
{
	pos = POS;
	hp = 3;

	prevHp = hp;
	redFlag = false;
	coolTimer = 0;
	coolTimeFlag = false;

	drawHpFlag = DRAW_UI_FLAG;

	larpTime = 0;

}

void Player::Finalize()
{
}

void Player::Input()
{
}

void Player::Update()
{

	//----------HPが減ったらプレイヤーを赤くする----------
	if (hp != prevHp)
	{
		//SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
		redFlag = true;
	}
	prevHp = hp;

	if (redFlag)
	{
		++coolTimer;

		if (COOL_MAX_TIME <= coolTimer)
		{
			redFlag = false;
			coolTimeFlag = false;
		}
	}
	else
	{
		coolTimer = 0;
	}

	if (redFlag)
	{
		//render->data.color.color.x = 255;
		//render->data.color.color.y = 0;
		//render->data.color.color.z = 0;
	}
	else
	{
		//render->data.color.color.x = 255;
		//render->data.color.color.y = 255;
		//render->data.color.color.z = 255;
	}
	//----------HPが減ったらプレイヤーを赤くする----------

	damageEffect.Update();


	//ImGui::Begin("Head");
	//KazImGuiHelper::InputVec3("Pos", &adjPos);
	//KazImGuiHelper::InputVec3("Rota", &adjRota);
	//ImGui::End();

	//プレイヤーのアニメーション制御
	float lNowRate = forceCameraRate + cameraRate.x * 0.5f;

	//左向き
	if (signbit(lNowRate))
	{
		leftFlag = true;
		rightFlag = false;
	}
	//右向き
	else
	{
		leftFlag = false;
		rightFlag = true;
	}

	//移動した方向をもとにプレイヤーの姿勢を求める。
	KazMath::Vec3<float> movedVec = pos - prevPos;
	//動いていたら姿勢を更新。動いていなかったらやばい値になるため。
	if (0 < movedVec.Length()) {

		KazMath::Vec3<float> movedVecNormal = movedVec.GetNormal();

		//デフォルトの回転軸と移動した方向のベクトルが同じ値だったらデフォルトの回転軸の方向に移動しているってこと！
		if (0.999f < movedVecNormal.Dot(KazMath::Vec3<float>(0, 0, 1))) {
			m_transform.rotation = {};
		}
		else {

			KazMath::Vec3<float> cameraAxisZ = movedVecNormal;
			KazMath::Vec3<float> cameraAxisY = KazMath::Vec3<float>(0, 1, 0);
			KazMath::Vec3<float> cameraAxisX = cameraAxisY.Cross(cameraAxisZ);
			cameraAxisY = cameraAxisZ.Cross(cameraAxisX);
			DirectX::XMMATRIX cameraMatWorld = DirectX::XMMatrixIdentity();
			cameraMatWorld.r[0] = { cameraAxisX.x, cameraAxisX.y, cameraAxisX.z, 0.0f };
			cameraMatWorld.r[1] = { cameraAxisY.x, cameraAxisY.y, cameraAxisY.z, 0.0f };
			cameraMatWorld.r[2] = { cameraAxisZ.x, cameraAxisZ.y, cameraAxisZ.z, 0.0f };
			DirectX::XMVECTOR rotate, scale, position;
			DirectX::XMMatrixDecompose(&scale, &rotate, &position, cameraMatWorld);
			m_transform.rotation = KazMath::Vec3<float>(DirectX::XMConvertToDegrees(rotate.m128_f32[0]), DirectX::XMConvertToDegrees(rotate.m128_f32[1]), DirectX::XMConvertToDegrees(rotate.m128_f32[2]));

		}

	}

	//前フレームの座標を保存。移動した方向を求めて姿勢制御に使用する。
	prevPos = pos;

	//for (auto& index : m_playerModel) {
	//	//DrawFunc::Test(index, m_transform, DrawFunc::NONE);

	//	m_emissive.x = 0.0f;
	//	m_emissive.y = 0.0f;
	//	m_emissive.z = 0.0f;
	//	m_emissive.a = 1;
	//	//index.extraBufferArray.back().bufferWrapper->TransData(&m_emissive, sizeof(DirectX::XMFLOAT4));
	//}

}

void Player::Draw(DrawingByRasterize& arg_rasterize, Raytracing::BlasVector& arg_blasVec)
{
	/*if (leftFlag)
	{
		fbxRender[LEFT].Draw();
	}
	else if (rightFlag)
	{
		fbxRender[RIGHT].Draw();
	}
	fbxRender[HEAD].Draw();

	damageEffect.Draw();
	damageWindow.Draw();

	*/

	//if (drawHpFlag)
	//{
	//	hpUi.Draw(arg_rasterize);
	//}

	m_transform.scale = { 50.0f,50.0f,50.0f };
	//m_transform.pos = { 0.0f,10.0f,50.0f };
	m_transform.rotation = { 0.0f,0.0f,0.0f };
	//bool test = true;
	//for (auto& index : m_playerModel) {
	//	//DrawFunc::DrawModel(index, m_transform);
	//	//arg_rasterize.ObjectRender(index);
	//	m_transform.pos = { 0.0f,10.0f,50.0f };
	//	m_transform.pos.x += KazMath::Rand(-30000.0f, 30000.0f);
	//	m_transform.scale = { 50.0f,50.0f,50.0f };
	//	m_transform.rotation = { 0.0f,0.0f,0.0f };
	//	for (auto& blas : index.m_raytracingData.m_blas) {
	//		arg_blasVec.Add(blas, m_transform.GetMat(), 0, test);
	//	}
	//	if (test) {
	//		test = false;
	//	}
	//}
	//m_transform.pos = { 0.0f,10.0f,50.0f };
	//m_transform.pos.x += KazMath::Rand(-300.0f, 300.0f);
	//m_transform.scale = { 50.0f,50.0f,50.0f };
	//m_transform.rotation = { 0.0f,0.0f,0.0f };
	//for (auto& blas : m_playerModel.back().m_raytracingData.m_blas) {
	//	arg_blasVec.AddTest(blas, m_transform.GetMat(), 100000);
	//}
}

void Player::Hit()
{
}

bool Player::IsAlive()
{
	return 0 < hp;
}
