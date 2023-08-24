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
	void Update();

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

	Sprite* spriteA_ = nullptr; // Aスプライト
	uint32_t textureA_ = 0u;    // Aテクスチャ

	Sprite* spriteB_ = nullptr; // Bスプライト
	uint32_t textureB_ = 0u;    // Bテクスチャ

	Sprite* spriteC_ = nullptr; // Cスプライト
	uint32_t textureC_ = 0u;    // Cテクスチャ
};
