#pragma once
#include"../Helper/ISinglton.h"

/// <summary>
/// fpsを管理します。
/// </summary>
class FpsManager final : public ISingleton<FpsManager> {
	//friend Singleton<FpsManager>;

	// フレーム数カウント
	static long long count_;
	// 基本fps値
	static int fps_limit_;
	// なにこれ
	static long long time_;
	// fps値出力
	static double fps_result_;
	// 現在時刻
	static long long current_time_;
	// 次の更新時間
	static long long next_;

public:

	static const int &GetFpsLimit() { return fps_limit_; }

	/// <summary>
	/// fpsを制限する
	/// </summary>
	/// <param name="fps_limit">fps上限</param>
	static void RegulateFps(const int fps_limit);

	/// <summary>
	/// タイトルバーにfpsを描画
	/// </summary>
	static void ObserveFPS();

private:
	/// <summary>
	/// 現在時刻を取得する
	/// </summary>
	/// <returns>現在時刻</returns>
	static long long CurrentTime();

	/// <summary>
	/// fpsを計算する
	/// </summary>
	static void CalculateFps();
};
