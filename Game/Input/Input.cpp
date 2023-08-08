#include "Input.h"

Input::Input():m_unInputFlag(false)
{
}

bool Input::Done()
{
	if (!m_unInputFlag)
	{
		return KeyBoradInputManager::Instance()->InputTrigger(DIK_SPACE) || ControllerInputManager::Instance()->InputTrigger(XINPUT_GAMEPAD_A);
	}
	else
	{
		return false;
	}
}

bool Input::Select()
{
	return false;
}

void Input::Update()
{
}
