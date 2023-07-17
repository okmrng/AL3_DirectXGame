#pragma once

#include <functional>

/// <summary>
/// 時限発動クラス
/// </summary>
class TimedCall 
{
public:
	TimedCall(std::function<void(void)> f, uint32_t time);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	// 完了ならtrueを返す
	bool IsFinished() { return finish_; }

private:
	// コールバック
	std::function<void(void)> f_;

	uint32_t time_;     // 残り時間

	bool finish_ = false; // 完了フラグ
};
