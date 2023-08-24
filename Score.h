#pragma once

#include <WorldTransform.h>
#include <Sprite.h>
#include <TextureManager.h>
#include <ImGuiManager.h>
#include <Audio.h>

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
	void Update(bool playSound);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI(bool drawRank);

	// スコア獲得
	void AddScore50();

	void AddScore100();

	void AddScore300();

	void AddScore();

	// スコア減点
	void SubtractScore();

private:
	Audio* audio_ = nullptr;

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

	uint32_t soundDamage_ = 0u; // ダメージを受けた時のサウンドハンドル
	uint32_t voiceDamage_ = 0u; // ダメージを受けた時の再生ハンドル

	// ランク
	Sprite* spriteRank_ = nullptr; // RANKスプライト
	uint32_t textureRank_ = 0u;    // RANKテクスチャ

	Sprite* spriteS_ = nullptr; // Sスプライト
	uint32_t textureS_ = 0u;    // Sテクスチャ

	uint32_t soundS_ = 0u;     // Sのサウンドハンドル
	uint32_t voiceS_ = 0u;     // Sの再生ハンドル
	int32_t stopCountS_ = 120; // 停止カウント

	Sprite* spriteA_ = nullptr; // Aスプライト
	uint32_t textureA_ = 0u;    // Aテクスチャ

	uint32_t soundA_ = 0u;    // Aのサウンドハンドル
	uint32_t voiceA_ = 0u;    // Aの再生ハンドル
	int32_t stopCountA_ = 60; // 停止カウント

	Sprite* spriteB_ = nullptr; // Bスプライト
	uint32_t textureB_ = 0u;    // Bテクスチャ

	uint32_t soundB_ = 0u;    // Bのサウンドハンドル
	uint32_t voiceB_ = 0u;    // Bの再生ハンドル
	int32_t stopCountB_ = 60; // 停止カウント

	Sprite* spriteC_ = nullptr; // Cスプライト
	uint32_t textureC_ = 0u;    // Cテクスチャ

	uint32_t soundC_ = 0u;   // Cのサウンドハンドル
	uint32_t voiceC_ = 0u;   // Cの再生ハンドル
	int32_t stopCountC_ = 60; // 停止カウント
};
