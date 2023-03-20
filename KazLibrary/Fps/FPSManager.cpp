#include "FPSManager.h"
#include <thread>
#include <chrono>
//#include "Window.h"

#pragma comment(lib, "winmm.lib")

long long FpsManager::count_ = 0;
int FpsManager::fps_limit_ = 0;
long long FpsManager::time_ = FpsManager::CurrentTime();
double FpsManager::fps_result_ = 0;
long long FpsManager::current_time_ = 0;
long long FpsManager::next_ = 0;

void FpsManager::RegulateFps(const int fps_limit) {

	// fps値を設定
	fps_limit_ = fps_limit;

	count_++;

	// 既定フレーム数になったらフレームレートの更新
	if (fps_limit_ <= count_) {
		CalculateFps();
	}

	current_time_ = CurrentTime();

	if (current_time_ < next_) {
		// 更新時間まで待機
		std::this_thread::sleep_for(std::chrono::microseconds(next_ - current_time_));

		// 次の更新時間を計算(1秒 / フレームレート加算)
		next_ += (1000 * 1000 / fps_limit_);
	} else {
		// 更新時間を過ぎた場合は現在時刻から次の更新時間を計算
		next_ = current_time_ + (1000 * 1000 / fps_limit_);
	}
}

long long FpsManager::CurrentTime() {
	using namespace std::chrono;

	system_clock::duration duration = system_clock::now().time_since_epoch();

	return duration_cast<microseconds>(duration).count();
}

void FpsManager::CalculateFps() {
	long long end = CurrentTime();

	fps_result_ = static_cast<double>(1000) / (end - time_) * count_;

	time_ = end;

	count_ = 0;
}

//void FpsManager::ObserveFPS() {
//	static DWORD prev_time = timeGetTime();	// 前回の時間
//	static int frame_count = 0;				// フレームカウント
//	DWORD now_time = timeGetTime();			// 今回のフレームの時間
//
//	frame_count++;							// フレーム数をカウントする
//
//	// 経過時間が１秒を超えたらカウントと時間をリセット
//	if (now_time - prev_time >= 1000) {
//		wchar_t fps[1000];
//		_itow_s(frame_count, fps, 10);
//		wchar_t moji[] = L"FPS";
//		wcscat_s(fps, moji);
//		SetWindowText(Window::GetHwnd(), fps);
//		//OutputDebugString(fps);
//
//		prev_time = now_time;
//		frame_count = 0;
//	}
//}
