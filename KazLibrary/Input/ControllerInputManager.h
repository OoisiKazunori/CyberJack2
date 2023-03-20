#pragma once
#include"../DirectXCommon/Base.h"
#include"IInputManager.h"
#include"../Helper/ISinglton.h"
#include<Xinput.h>
#pragma comment(lib, "Xinput.lib")
/*
パッド操作
スティック操作全般
*/

enum ControllerStickSide
{
	RIGHT_STICK,
	LEFT_STICK
};

enum ControllerSide
{
	UP_SIDE,
	DOWN_SIDE,
	LEFT_SIDE,
	RIGHT_SIDE
};

enum ControllerShoulderSide
{
	LEFT_SHOULDER,
	RIGHT_SHOULDER
};

//最大四つまでコントローラーは使用可能らしいが、現在二つ同時動作確認済み
//Stickの判定全般にてスティックを傾けた状態で指を離すと時々逆方向の判定を取る
//恐らく純正コントローラーのみ対応可能

class ControllerInputManager :public ISingleton<ControllerInputManager>, IInputManager
{
public:
	ControllerInputManager();
	~ControllerInputManager();

	/// <summary>
	/// ループの始めの方においてください
	/// </summary>
	void InputLog()override;
	
	//ボタン操作

	bool InputTrigger(int BUTTON)override;
	bool InputState(int BUTTON)override;
	bool InputRelease(int BUTTON)override;


	//スティック操作

	bool InputStickTrigger(ControllerStickSide STICK_TYPE,ControllerSide SIDE, int DEAD_ZONE = 6000);	
	bool InputStickState(ControllerStickSide STICK_TYPE, ControllerSide SIDE, int DEAD_ZONE = 6000);
	bool InputStickRelease(ControllerStickSide STICK_TYPE, ControllerSide SIDE, int DEAD_ZONE = 6000);
	//X軸スティックの傾き値
	const int GetJoyStickLXNum(int PAD_NUMBER = 0);
	//Y軸スティックの傾き値
	const int GetJoyStickLYNum(int PAD_NUMBER = 0);

	//X軸スティックの傾き値
	const int GetJoyStickRXNum(int PAD_NUMBER = 0);
	//Y軸スティックの傾き値
	const int GetJoyStickRYNum(int PAD_NUMBER = 0);


	//トリガー

	bool InputShoulderTrigger(ControllerShoulderSide SIDE, int DEAD_ZONE = 100);	
	bool InputShoulderState(ControllerShoulderSide SIDE, int DEAD_ZONE = 100);
	bool InputShoulderRelease(ControllerShoulderSide SIDE, int DEAD_ZONE = 100);


private:
	XINPUT_STATE state[XUSER_MAX_COUNT], oldState[XUSER_MAX_COUNT];

	//最初の一フレーム目でRelease判定が勝手に通ってしまうのを防ぐ
	bool initReleaseFlag;
	int roopCount;
};

