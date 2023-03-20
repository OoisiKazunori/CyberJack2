#pragma once
#include"../DirectXCommon/Base.h"
#include"IInputManager.h"
#include"../Helper/ISinglton.h"

//DirextInputのバージョン指定
#define DIRECTINPUT_VERSION 0x0800

#include<dinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

/*
主な役割
キーボードとマウス入力関連
マウスの座標は取得できない
*/

enum MouseInputNumber
{
	MOUSE_INPUT_LEFT,
	MOUSE_INPUT_RIGHT,
	MOUSE_INPUT_MIDDLE,
};

class KeyBoradInputManager:public ISingleton<KeyBoradInputManager>
{
public:
	KeyBoradInputManager();
	~KeyBoradInputManager();

	void CreateDevice(HWND *HWND, HINSTANCE *INSTANCE);

	/// <summary>
	/// ループの始めの方においてください
	/// </summary>
	void InputLog();

	bool InputTrigger(int BUTTON);
	bool InputState(int BUTTON);
	bool InputRelease(int BUTTON);

	bool MouseInputTrigger(MouseInputNumber CLICK);
	bool MouseInputState(MouseInputNumber CLICK);
	bool MouseInputRelease(MouseInputNumber CLICK);
	DirectX::XMFLOAT2 GetMousePoint();
	
private:
	HRESULT result;
	//入力デバイス
	IDirectInput8 *dinput;

	//キーボード
	IDirectInputDevice8 *devKeyBoard;
	char key[256], oldKey[256];
	int timer;
	int flame;

	//マウス
	IDirectInputDevice8 *devMouse;
	DIMOUSESTATE2 mouseState = {}, oldMouseState = {};
	HWND *hwnd;


	bool initReleaseFlag;
	int roopCount;
};

