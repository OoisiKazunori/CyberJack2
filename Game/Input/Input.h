#pragma once
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"../../KazLibrary/Helper/ISinglton.h"

class Input:public ISingleton<Input>
{
public:
	Input();
	bool Done();
	bool Select();

	void Update();

	bool m_unInputFlag;//“ü—Í’âŽ~
};

