#include "Player.h"
#include"../KazLibrary/Imgui/MyImgui.h"
#include"../KazLibrary/Sound/SoundManager.h"
#include"../KazLibrary/Helper/ResourceFilePass.h"

const int Player::COOL_MAX_TIME = 120;

Player::Player()
{
	hp = -1;
	pos = {};

	damageSoundHandle = SoundManager::Instance()->LoadSoundMem(KazFilePathName::SoundPath + "PlayerDamage.wav");

	fbxRender[LEFT].data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Right_Back_Anim.fbx");
	fbxRender[RIGHT].data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Left_Back_Anim.fbx");
	fbxRender[HEAD].data.handle = FbxModelResourceMgr::Instance()->LoadModel(KazFilePathName::PlayerPath + "CH_Model_Head.fbx");

	for (int i = 0; i < fbxRender.size(); ++i)
	{
		fbxRender[i].data.pipelineName = PIPELINE_NAME_FBX_RENDERTARGET_TWO;
		fbxRender[i].data.stopAnimationFlag = true;
		m_transform.scale = { 0.0f,0.0f,0.0f };
	}


	totalTime = FbxModelResourceMgr::Instance()->GetResourceData(fbxRender[0].data.handle.handle)->endTime[0] - FbxModelResourceMgr::Instance()->GetResourceData(fbxRender[0].data.handle.handle)->startTime[0];


	float lScale = 0.5f;
	minScale = { lScale ,lScale ,lScale };
	sinTimer = 0.0f;

	adjPos = { 0.0f,0.9f,0.3f };


	DrawFuncData::PipelineGenerateData lData;
	lData.desc = DrawFuncPipelineData::SetPosUvNormalTangentBinormal();
	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "VSPosNormalUvLightMain", "vs_6_4", SHADER_TYPE_VERTEX);
	lData.shaderDataArray.emplace_back(KazFilePathName::RelativeShaderPath + "ShaderFile/" + "Model.hlsl", "PSPosNormalUvLightMain", "ps_6_4", SHADER_TYPE_PIXEL);
	lData.blendMode = DrawFuncPipelineData::PipelineBlendModeEnum::ALPHA;
	m_playerModel = DrawFuncData::SetDrawGLTFIndexMaterialData(*ModelLoader::Instance()->Load("Resource/Test/glTF/Avocado/", "Avocado.gltf"), lData);
}

void Player::Init(const KazMath::Vec3<float>& POS, bool DRAW_UI_FLAG, bool APPEAR_FLAG)
{
	pos = POS;
	if (APPEAR_FLAG)
	{
		for (int i = 0; i < fbxRender.size(); ++i)
		{
			fbxRender[i].data.colorData.color.a = 0;
			m_transform.scale = { 0.5f,0.5f,0.5f };
		}
	}
	else
	{
		for (int i = 0; i < fbxRender.size(); ++i)
		{
			fbxRender[i].data.colorData.color.a = 255;
			m_transform.scale = { 0.5f,0.5f,0.5f };
		}
	}
	hp = 3;

	hpUi.Init(hp);
	prevHp = hp;
	redFlag = false;
	coolTimer = 0;
	coolTimeFlag = false;

	drawHpFlag = DRAW_UI_FLAG;

	larpTime = 0;

}

void Player::Finalize()
{
	damageWindow.Finalize();
}

void Player::Input()
{
}

void Player::Update()
{
	hpUi.Update();

	//----------HPが減ったらプレイヤーを赤くする----------
	if (hp != prevHp)
	{
		SoundManager::Instance()->PlaySoundMem(damageSoundHandle, 1);
		damageEffect.Init(pos);
		damageWindow.Init(pos + KazMath::Vec3<float>(WIN_X / 2.0f, WIN_Y / 2.0f + 200.0f, 0.0f));
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
	damageWindow.Update();

	for (int i = 0; i < fbxRender.size(); ++i)
	{
		fbxRender[i].data.colorData.color.a += 255 / 120;
		if (255 <= fbxRender[i].data.colorData.color.a)
		{
			fbxRender[i].data.colorData.color.a = 255;
		}
	}


	//ImGui::Begin("Head");
	//KazImGuiHelper::InputVec3("Pos", &adjPos);
	//KazImGuiHelper::InputVec3("Rota", &adjRota);
	//ImGui::End();

	fbxRender[HEAD].data.transform.rotation = adjRota;

	for (int i = 0; i < fbxRender.size() - 1; ++i)
	{
		m_transform.pos = pos + KazMath::Vec3<float>(0.0f, 1.0f + sinf(KazMath::PI_2F / 120.0f * sinTimer) * 0.2f, 0.0f);
	}

	fbxRender[HEAD].data.transform.pos = pos + adjPos + KazMath::Vec3<float>(0.0f, 1.0f + sinf(KazMath::PI_2F / 120.0f * sinTimer) * 0.2f, 0.0f);

	++sinTimer;
	++larpTime;
	if (30 <= larpTime)
	{
		float lScale = 0.55f;
		for (int i = 0; i < fbxRender.size(); ++i)
		{
			m_transform.scale = { lScale,lScale,lScale + 0.1f };
		}
		larpTime = 0;
	}

	for (int i = 0; i < fbxRender.size(); ++i)
	{
		KazMath::Larp(minScale, &m_transform.scale, 0.1f);
	}


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


	if (leftFlag)
	{
		float lTotalTime = static_cast<float>(totalTime.Get());
		FbxLongLong convertTime = static_cast<FbxLongLong>(lTotalTime * -lNowRate);
		FbxTime lNowTime = convertTime;
		fbxRender[LEFT].currentTime = lNowTime;

		adjPos.x = 0.0f + -cameraRate.x * 0.2f;

		adjRota.x = 50.0f * -cameraRate.y;
		adjRota.y = -52.0f * -cameraRate.x;
		adjRota.z = 0.0f;
	}
	else if (rightFlag)
	{
		float lTotalTime = static_cast<float>(totalTime.Get());
		FbxLongLong convertTime = static_cast<FbxLongLong>(lTotalTime * lNowRate);
		FbxTime lNowTime = convertTime;
		fbxRender[RIGHT].currentTime = lNowTime;

		adjRota.x = 0.0f;
		adjRota.y = -270.0f + 60.0f * cameraRate.x;
		adjRota.z = 15.0f + 75.0f * -cameraRate.y;
	}

	//移動した方向をもとにプレイヤーの姿勢を求める。
	KazMath::Vec3<float> movedVec = pos - prevPos;
	//動いていたら姿勢を更新。動いていなかったらやばい値になるため。
	if (0 < movedVec.Length()) {

		//デフォルトの回転軸と移動した方向のベクトルが同じ値だったらデフォルトの回転軸の方向に移動しているってこと！
		if (0.999f < movedVec.GetNormal().Dot(KazMath::Vec3<float>(0, 0, 1))) {
			m_transform.rotation = {};
		}
		else {
			//回転を求めるためにまずはデフォルトのベクトルと移動方向の回転軸を求める。
			KazMath::Vec3<float> moveRotationAxis = movedVec.GetNormal().Cross(KazMath::Vec3<float>(0, 0, 1));
			//次は回転量
			float moveRotationAngle = acosf(movedVec.GetNormal().Dot(KazMath::Vec3<float>(0, 0, 1)));

			//回転させる。
			DirectX::XMVECTOR playerQ = DirectX::XMQuaternionRotationAxis(DirectX::XMVectorSet(moveRotationAxis.x, moveRotationAxis.y, moveRotationAxis.z, 0.0f), moveRotationAngle);

			//回転をオイラー角に直す。
			DirectX::XMVECTOR scale, position, rotate;
			DirectX::XMMatrixDecompose(&scale, &rotate, &position, DirectX::XMMatrixRotationQuaternion(playerQ));
			m_transform.rotation = KazMath::Vec3<float>(rotate.m128_f32[0], rotate.m128_f32[1], rotate.m128_f32[2]);

		}

	}

	//前フレームの座標を保存。移動した方向を求めて姿勢制御に使用する。
	prevPos = pos;

}

void Player::Draw(DrawingByRasterize& arg_rasterize)
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

	if (drawHpFlag)
	{
		hpUi.Draw();
	}*/
	m_transform.scale = { 50.0f,50.0f,50.0f };
	DrawFunc::DrawModel(m_playerModel, m_transform);
	arg_rasterize.ObjectRender(m_playerModel);
}

void Player::Hit()
{
	if (!coolTimeFlag)
	{
		--hp;
		hpUi.Sub();
	}
	coolTimeFlag = true;
}

bool Player::IsAlive()
{
	return 0 < hp;
}
