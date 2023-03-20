#pragma once
#include<Windows.h>
#include<string>

/// <summary>
/// ウィンドウの作成をします
/// </summary>
class WinApi 
{
public:
	WinApi();

	/// <summary>
	/// ウィンドウの作成をします
	/// </summary>
	/// <param name="WINDOW_SIZE_X">ウィンドウのXサイズ</param>
	/// <param name="WINDOW_SIZE_Y">ウィンドウのYサイズ</param>
	/// <returns>true...成功,false...失敗</returns>
	HRESULT CreateMyWindow(int WINDOW_SIZE_X,int WINDOW_SIZE_Y);


	void FPS()
	{
		static DWORD prev_time = timeGetTime();    // 前回の時間
		static int frame_count = 0;        // フレームカウント
		DWORD now_time = timeGetTime();        // 今回のフレームの時間

		frame_count++;    // フレーム数をカウントする

		// 経過時間が１秒を超えたらカウントと時間をリセット
		if (now_time - prev_time >= 1000)
		{
			wchar_t fps[1000];
			_itow_s(frame_count, fps, 10);
			wchar_t moji[] = L"FPS";
			wcscat_s(fps, moji);
			SetWindowText(hwnd, fps);
			//OutputDebugString(fps);

			prev_time = now_time;
			frame_count = 0;
		}
	}


	/// <summary>
	/// ウィンドウクラス登録解除
	/// </summary>
	void UnregisterWindowClass();

	HWND hwnd;
	WNDCLASSEX window{};
private:
};

