#include"CameraWork.h"
#include"../KazLibrary/Input/KeyBoradInputManager.h"

CameraWork::CameraWork()
{
	besidePoly = std::make_unique<BoxPolygonRender>();
	verticlaPoly = std::make_unique<BoxPolygonRender>();
	cameraPoly = std::make_unique<BoxPolygonRender>();
}

void CameraWork::Init(const KazMath::Vec3<float> &BASE_POS)
{
	centralPos = BASE_POS;
	baseTargetPos = KazMath::Vec3<float>(0.0f, 3.0f, 0.0f);


	eyePos = { 0.0f,5.0f,-10.0f };
	leftRightAngleVel = { -90.0f,-90.0f };
	upDownAngleVel = { 0.0f,0.0f };
	mulValue = { 10.0f,30.0f };
	mulValue2 = { 60.0f,60.0f };


	trackLeftRightAngleVel = leftRightAngleVel;
	trackUpDownAngleVel = upDownAngleVel;

	r = 8.0f;
	r2 = 8.0f;

	forceCameraDirVel.x = FORCE_CAMERA_FRONT;
}

void CameraWork::Update(const KazMath::Vec2<float> &CURSOR_VALUE, KazMath::Vec3<float> *PLAYER_POS, bool DEBUG_FLAG)
{
	if (!DEBUG_FLAG)
	{
#pragma region GameCamera

		upDownAngleVel =
		{
			mulValue.x * -CURSOR_VALUE.y,
			mulValue.y * -CURSOR_VALUE.y
		};
		leftRightAngleVel =
		{
			forceCameraDirVel.x + mulValue2.x * -CURSOR_VALUE.x,
			forceCameraDirVel.x + mulValue2.y * -CURSOR_VALUE.x
		};

		//左右の角度変更のイージング
		{
			KazMath::Vec2<float> distance = leftRightAngleVel - trackLeftRightAngleVel;
			trackLeftRightAngleVel += distance * 0.1f;
		}
		//上下の角度変更のイージング
		{
			KazMath::Vec2<float> distance = upDownAngleVel - trackUpDownAngleVel;
			trackUpDownAngleVel += distance * 0.1f;
		}
		//左右の回転
		besidePoly->data.transform.pos =
		{
			cosf(KazMath::AngleToRadian(trackLeftRightAngleVel.x)) * r,
			0.0f,
			sinf(KazMath::AngleToRadian(trackLeftRightAngleVel.y)) * r
		};
		//上下の回転
		verticlaPoly->data.transform.pos =
		{
			0.0f,
			trackUpDownAngleVel.x,
			0.0f,
		};

		{
			float mul = 1.0f;
			float limitValue = 3.0f;
			//上下でプレイヤーのいる位置を変える
			if (0.1f <= CURSOR_VALUE.y)
			{
				mul *= -1.0f;
				limitValue = 2.5f;
			}

			//ポータル演出開始したらプレイヤーの動きを固定する
			DirectX::XMFLOAT3 rate = { CURSOR_VALUE.x ,CURSOR_VALUE.y,0.0f };
			honraiPlayerCameraPos.x = 0.0f + (2.0f * -rate.x);
			honraiPlayerCameraPos.z = 0.0f + (limitValue * rate.y) * mul;

			if (0.0f <= honraiPlayerCameraPos.z)
			{
				honraiPlayerCameraPos.z = 0.0f;
			}
			KazMath::Vec3<float> distance = honraiPlayerCameraPos - *PLAYER_POS;
			*PLAYER_POS += distance * 0.1f;
		}

		//上下左右の回転
		cameraPoly->data.transform.pos = centralPos + (besidePoly->data.transform.pos + verticlaPoly->data.transform.pos);
#pragma endregion
		eyePos = cameraPoly->data.transform.pos;
		targetPos = baseTargetPos;
	}
	else
	{
#pragma region DebugCamera
		KeyBoradInputManager *input = KeyBoradInputManager::Instance();

		debugCameraMove = { 0,0,0 };
		float debugSpeed = 10000.0f;
		float debugSideSpeed = 10.0f;
		if (input->InputState(DIK_D))
		{
			debugCameraMove.x = -debugSpeed;
		}
		if (input->InputState(DIK_A))
		{
			debugCameraMove.x = debugSpeed;
		}
		if (input->InputState(DIK_W))
		{
			debugCameraMove.y = debugSpeed;
		}
		if (input->InputState(DIK_S))
		{
			debugCameraMove.y = -debugSpeed;
		}

		if (input->InputState(DIK_RIGHTARROW))
		{
			angle.x += debugSideSpeed;
		}
		if (input->InputState(DIK_LEFTARROW))
		{
			angle.x += -debugSideSpeed;
		}

		if (input->InputState(DIK_UPARROW))
		{
			angle.y += debugSideSpeed;
		}
		if (input->InputState(DIK_DOWNARROW))
		{
			angle.y += -debugSideSpeed;
		}
#pragma endregion
		//デバック用
		eyePos = KazMath::CaluEyePosForDebug(eyePos, debugCameraMove, angle, 0.1f);
		targetPos = KazMath::CaluTargetPosForDebug(eyePos, angle.x);
	}


}

const KazMath::Vec3<float> CameraWork::GetEyePos()
{
	return eyePos;
}

const KazMath::Vec3<float> CameraWork::GetTargetPos()
{
	return targetPos;
}

void CameraWork::ForceCamera(KazEnemyHelper::CameraDir DIR)
{
	switch (DIR)
	{
	case KazEnemyHelper::CAMERA_FRONT:
		forceCameraDirVel.x = FORCE_CAMERA_FRONT;
		break;
	case KazEnemyHelper::CAMERA_BACK:
		forceCameraDirVel.x = FORCE_CAMERA_BACK;
		break;
	case KazEnemyHelper::CAMERA_LEFT:
		forceCameraDirVel.x = FORCE_CAMERA_LEFT;
		break;
	case KazEnemyHelper::CAMERA_RIGHT:
		forceCameraDirVel.x = FORCE_CAMERA_RIGHT;
		break;
	default:
		break;
	}
}