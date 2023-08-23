#pragma once

#include <WorldTransform.h>
#include <Sprite.h>
#include <TextureManager.h>
#include <ImGuiManager.h>

/// <summary>
/// スコア
/// </summary>
class Score {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Score();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI(bool drawHighScore);

	// スコア獲得
	void AddScore50();

	void AddScore100();

	void AddScore300();

	void AddScore();

	// スコア減点
	void SubtractScore();

private:
	// SCORE
	WorldTransform worldTransform_; // ワールド変換

	Sprite* sprite_ = nullptr;    // スプライト
	uint32_t textureHandle_ = 0u; // テクスチャハンドル

	// number
	WorldTransform worldTransformNumber_; // ワールド変換

	Sprite* spriteNumber_[5]; // スプライト
	uint32_t textureHandleNumber_[10] = {0u, 0u, 0u, 0u, 0u,
	                                     0u, 0u, 0u, 0u, 0u}; // テクスチャハンドル
	int32_t width_ = 39;                                      // 画像の横幅

	int32_t score_ = 0;           // スコア
	int32_t subtractScore_ = 50;  // スコア減算
	bool add_ = false;            // スコア減算量を二倍にするフラグ

	int32_t eachNumber_[5] = {0}; // 画像の割り当て
	int32_t number_ = score_;     // スコアの値で画像を割り当てる
};
