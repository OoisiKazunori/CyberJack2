#pragma once
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"../../KazLibrary/Helper/ISinglton.h"

class Input:public ISingleton<Input>
{
public:
	Input();
	//�A�E�g�Q�[���ł̑���-----------------------------
	bool Done();
	bool Select();
	//�A�E�g�Q�[���ł̑���-----------------------------

	//�C���Q�[���ł̑���-----------------------------

	//�C���Q�[���ł̑���-----------------------------


	void Update();

	bool m_unInputFlag;//���͒�~
};

