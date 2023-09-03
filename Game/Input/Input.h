#pragma once
#include"../KazLibrary/Input/KeyBoradInputManager.h"
#include"../KazLibrary/Input/ControllerInputManager.h"
#include"../../KazLibrary/Helper/ISinglton.h"

class Input:public ISingleton<Input>
{
public:
	Input();
	//アウトゲームでの操作-----------------------------
	bool Done();
	bool Select();
	//アウトゲームでの操作-----------------------------

	//インゲームでの操作-----------------------------

	//インゲームでの操作-----------------------------


	void Update();

	bool m_unInputFlag;//入力停止
};

