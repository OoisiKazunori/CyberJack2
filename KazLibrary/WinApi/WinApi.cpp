#include "WinApi.h"
#include"../Imgui/imgui_impl_win32.h"

//Imguiのメニューを動かしたり、サイズ変更するために必要
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

//DISARDBLE...メモリからアイコンを破棄しても良い事を示す
#define KITTY ICON DISCARDABLE "icon.rc"


LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) 
{
	switch (msg) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam);
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



WinApi::WinApi() 
{
	//hwnd = nullptr;
}

HRESULT WinApi::CreateMyWindow(int WINDOW_SIZE_X, int WINDOW_SIZE_Y) 
{
	int CheckError = 0;

	if (WINDOW_SIZE_X <= 0 || WINDOW_SIZE_Y <= 0)
	{
		return S_FALSE;
	}

	//Win = WindowProc;


	/*
	色んなウィンドウの設定
	一行目、ウィンドウクラス
	三行目、ウィンドウプロシージャ
	四行目、ウィンドウハンドル
	五行目、カーソル指定
	*/

	window.cbSize = sizeof(WNDCLASSEX);
	window.lpfnWndProc = (WNDPROC)WindowProc;
	window.lpszClassName = L"DirectX";
	window.hInstance = GetModuleHandle(nullptr);
	window.hCursor = LoadCursor(NULL, IDC_ARROW);	
	window.hIcon = LoadIcon(window.hInstance, TEXT("KITTY"));

	

	//ウィンドウクラスをOSに登録
	CheckError = RegisterClassEx(&window);
	if (!CheckError) 
	{
		return S_FALSE;
	}

	//ウィンドウサイズ作成
	RECT wrc = { 0,0,WINDOW_SIZE_X,WINDOW_SIZE_Y };
	//自動でサイズ補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);


	/*
	ウィンドウオブジェクトの生成

	上から順にCreateWinodwの引数の役割

	ウィンドウのクラス名
	タイトルバーの文字列
	標準的なウィンドウスタイル
	表示X座標(OSに任せる)
	表示Y座標(OSに任せる)
	ウィンドウ横幅
	ウィンドウ縦幅
	親ウィンドウハンドル
	メニューハンドル
	呼び出しアプリケーション
	オプション
	*/
	
	hwnd = CreateWindow(
		window.lpszClassName,
		L"CyberJack",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		window.hInstance,
		nullptr
	);



	//ウィンドウ表示
	CheckError = ShowWindow(hwnd, SW_SHOW);
	if (!CheckError) 
	{
		return S_OK;
	}

	return S_FALSE;
}

void WinApi::UnregisterWindowClass()
{
	//ウィンドウクラス登録解除
	UnregisterClass(window.lpszClassName, window.hInstance);
}

