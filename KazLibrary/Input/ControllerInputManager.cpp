#include "ControllerInputManager.h"
#include"../Helper/KazHelper.h"

ControllerInputManager::ControllerInputManager() {
	roopCount = 0;
	initReleaseFlag = false;
}

ControllerInputManager::~ControllerInputManager() {
}

void ControllerInputManager::InputLog()
{
	//最初の一フレーム目でリリースの処理が入るのを防ぐ
	if (2 <= roopCount && !initReleaseFlag)
	{
		initReleaseFlag = true;
	}
	else
	{
		roopCount++;
	}

	DWORD dwResult;
	for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
	{
		oldState[i] = state[i];

		ZeroMemory(&state[i], sizeof(XINPUT_STATE));
		//戻り値でコントローラーが接続さえれているかどうか確認できる
		dwResult = XInputGetState(i, &state[i]);

		if (dwResult == ERROR_SUCCESS)
		{
			bool okFlag;
			okFlag = false;
		}
		else
		{
			bool okFlag;
			okFlag = true;
		}
	}

}

bool ControllerInputManager::InputTrigger(int BUTTON)
{
	if (!(oldState[0].Gamepad.wButtons & BUTTON) && (state[0].Gamepad.wButtons & BUTTON))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ControllerInputManager::InputState(int BUTTON)
{
	if ((oldState[0].Gamepad.wButtons & BUTTON) && (state[0].Gamepad.wButtons & BUTTON))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ControllerInputManager::InputRelease(int BUTTON)
{
	if (initReleaseFlag)
	{
		if ((oldState[0].Gamepad.wButtons & BUTTON) && !(state[0].Gamepad.wButtons & BUTTON))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool ControllerInputManager::InputStickTrigger(ControllerStickSide STICK_TYPE, ControllerSide SIDE, int DEAD_ZONE)
{
	if (STICK_TYPE == LEFT_STICK)
	{
#pragma region 左スティック
		switch (SIDE)
		{
		case UP_SIDE:
			if (!(DEAD_ZONE <= oldState[0].Gamepad.sThumbLY) && (DEAD_ZONE <= state[0].Gamepad.sThumbLY))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case DOWN_SIDE:
			if (!(oldState[1].Gamepad.sThumbLY <= -DEAD_ZONE) && (state[1].Gamepad.sThumbLY <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case LEFT_SIDE:
			if (!(oldState[0].Gamepad.sThumbLX <= -DEAD_ZONE) && (state[0].Gamepad.sThumbLX <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case RIGHT_SIDE:
			if (!(DEAD_ZONE <= oldState[0].Gamepad.sThumbLX) && (DEAD_ZONE <= state[0].Gamepad.sThumbLX))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		default:
			break;
		}
#pragma endregion

	}
	else
	{
#pragma region 右スティック
		switch (SIDE)
		{
		case UP_SIDE:
			if (!(DEAD_ZONE <= oldState[0].Gamepad.sThumbRY) && (DEAD_ZONE <= state[0].Gamepad.sThumbRY))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case DOWN_SIDE:
			if (!(oldState[1].Gamepad.sThumbRY <= -DEAD_ZONE) && (state[1].Gamepad.sThumbRY <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case LEFT_SIDE:
			if (!(oldState[0].Gamepad.sThumbRX <= -DEAD_ZONE) && (state[0].Gamepad.sThumbRX <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case RIGHT_SIDE:
			if (!(DEAD_ZONE <= oldState[0].Gamepad.sThumbRX) && (DEAD_ZONE <= state[0].Gamepad.sThumbRX))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		default:
			break;
		}

#pragma endregion
	}

	return false;
}

bool ControllerInputManager::InputStickState(ControllerStickSide STICK_TYPE, ControllerSide SIDE, int DEAD_ZONE)
{
	if (STICK_TYPE == LEFT_STICK)
	{
#pragma region 左スティック
		switch (SIDE)
		{
		case UP_SIDE:
			if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbLY) && (DEAD_ZONE <= state[0].Gamepad.sThumbLY))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case DOWN_SIDE:
			if ((oldState[0].Gamepad.sThumbLY <= -DEAD_ZONE) && (state[0].Gamepad.sThumbLY <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case LEFT_SIDE:
			if ((oldState[0].Gamepad.sThumbLX <= -DEAD_ZONE) && (state[0].Gamepad.sThumbLX <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case RIGHT_SIDE:
			if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbLX) && (DEAD_ZONE <= state[0].Gamepad.sThumbLX))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		default:
			break;
		}
#pragma endregion
	}
	else
	{
#pragma region 右スティック
		switch (SIDE)
		{
		case UP_SIDE:
			if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbRY) && (DEAD_ZONE <= state[0].Gamepad.sThumbRY))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case DOWN_SIDE:
			if ((oldState[0].Gamepad.sThumbRY <= -DEAD_ZONE) && (state[0].Gamepad.sThumbRY <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case LEFT_SIDE:
			if ((oldState[0].Gamepad.sThumbRX <= -DEAD_ZONE) && (state[0].Gamepad.sThumbRX <= -DEAD_ZONE))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		case RIGHT_SIDE:
			if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbRX) && (DEAD_ZONE <= state[0].Gamepad.sThumbRX))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;



		default:
			break;
		}
#pragma endregion
	}
	
	return false;
}

bool ControllerInputManager::InputStickRelease(ControllerStickSide STICK_TYPE, ControllerSide SIDE, int DEAD_ZONE)
{
	if (initReleaseFlag)
	{

		if (STICK_TYPE == LEFT_STICK)
		{
#pragma region 左スティック
			switch (SIDE)
			{
			case UP_SIDE:
				if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbLY) && !(DEAD_ZONE <= state[0].Gamepad.sThumbLY))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			case DOWN_SIDE:
				if ((oldState[0].Gamepad.sThumbLY <= -DEAD_ZONE) && !(state[0].Gamepad.sThumbLY <= -DEAD_ZONE))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			case LEFT_SIDE:
				if ((oldState[0].Gamepad.sThumbLX <= -DEAD_ZONE) && !(state[0].Gamepad.sThumbLX <= -DEAD_ZONE))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			case RIGHT_SIDE:
				if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbLX) && !(DEAD_ZONE <= state[0].Gamepad.sThumbLX))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			default:
				break;
			}
#pragma endregion
		}
		else
		{
#pragma region 右スティック
			switch (SIDE)
			{
			case UP_SIDE:
				if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbRY) && !(DEAD_ZONE <= state[0].Gamepad.sThumbRY))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			case DOWN_SIDE:
				if ((oldState[0].Gamepad.sThumbRY <= -DEAD_ZONE) && !(state[0].Gamepad.sThumbRY <= -DEAD_ZONE))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			case LEFT_SIDE:
				if ((oldState[0].Gamepad.sThumbRX <= -DEAD_ZONE) && !(state[0].Gamepad.sThumbRX <= -DEAD_ZONE))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			case RIGHT_SIDE:
				if ((DEAD_ZONE <= oldState[0].Gamepad.sThumbRX) && !(DEAD_ZONE <= state[0].Gamepad.sThumbRX))
				{
					return true;
				}
				else
				{
					return false;
				}
				break;



			default:
				break;
			}
#pragma endregion
		}
	}
	return false;
}

const int ControllerInputManager::GetJoyStickLXNum(int PAD_NUMBER)
{
	if (PAD_NUMBER <= -1 && 4 <= PAD_NUMBER)
	{
		FailCheck("コントローラーの番号を取得できず、失敗しました");
		return -1;
	}
	return state[PAD_NUMBER].Gamepad.sThumbLX;
}

const int ControllerInputManager::GetJoyStickLYNum(int PAD_NUMBER)
{
	if (PAD_NUMBER <= -1 && 4 <= PAD_NUMBER)
	{
		FailCheck("コントローラーの番号を取得できず、失敗しました");
		return -1;
	}
	return state[PAD_NUMBER].Gamepad.sThumbLY;
}

const int ControllerInputManager::GetJoyStickRXNum(int PAD_NUMBER)
{
	if (PAD_NUMBER <= -1 && 4 <= PAD_NUMBER)
	{
		FailCheck("コントローラーの番号を取得できず、失敗しました");
		return -1;
	}
	return state[PAD_NUMBER].Gamepad.sThumbRX;
}

const int ControllerInputManager::GetJoyStickRYNum(int PAD_NUMBER)
{
	if (PAD_NUMBER <= -1 && 4 <= PAD_NUMBER)
	{
		FailCheck("コントローラーの番号を取得できず、失敗しました");
		return -1;
	}
	return state[PAD_NUMBER].Gamepad.sThumbRY;
}

bool ControllerInputManager::InputShoulderTrigger(ControllerShoulderSide SIDE, int DEAD_ZONE)
{
	switch (SIDE)
	{
	case LEFT_SHOULDER:
		if (!(DEAD_ZONE <= oldState[0].Gamepad.bLeftTrigger) && (DEAD_ZONE <= state[0].Gamepad.bLeftTrigger))
		{
			return true;
		}
		else
		{
			return false;
		}
		break;


	case RIGHT_SHOULDER:
		if (!(DEAD_ZONE <= oldState[0].Gamepad.bRightTrigger) && (DEAD_ZONE <= state[0].Gamepad.bRightTrigger))
		{
			return true;
		}
		else
		{
			return false;
		}
		break;


	default:
		break;
	}

	return false;
}

bool ControllerInputManager::InputShoulderState(ControllerShoulderSide SIDE, int DEAD_ZONE)
{
	switch (SIDE)
	{
	case LEFT_SHOULDER:
		if ((DEAD_ZONE <= oldState[0].Gamepad.bLeftTrigger) && (DEAD_ZONE <= state[0].Gamepad.bLeftTrigger))
		{
			return true;
		}
		else
		{
			return false;
		}
		break;


	case RIGHT_SHOULDER:
		if ((DEAD_ZONE <= oldState[0].Gamepad.bRightTrigger) && (DEAD_ZONE <= state[0].Gamepad.bRightTrigger))
		{
			return true;
		}
		else
		{
			return false;
		}
		break;

	default:
		break;
	}

	return false;

}

bool ControllerInputManager::InputShoulderRelease(ControllerShoulderSide SIDE, int DEAD_ZONE)
{
	if (initReleaseFlag) {

		switch (SIDE)
		{
		case LEFT_SHOULDER:
			if ((DEAD_ZONE <= oldState[0].Gamepad.bLeftTrigger) && !(DEAD_ZONE <= state[0].Gamepad.bLeftTrigger))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;


		case RIGHT_SHOULDER:
			if ((DEAD_ZONE <= oldState[0].Gamepad.bRightTrigger) && !(DEAD_ZONE <= state[0].Gamepad.bRightTrigger))
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		default:
			break;
		}

	}

	return false;

}
