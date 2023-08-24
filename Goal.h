#pragma once

#include <Model.h>
#include <cassert>
#include <Sprite.h>
#include "MathUtility.h"
#include <Input.h>
#include "Audio.h"

/// <summary>
/// ゴール
/// </summary>
class Goal {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 当たった時の処理
	/// </summary>
	void OnColision();

	// ゲッター関数
	/// <summary>
	/// 半径
	/// </summary>
	float GetRadius() { return radius_; }

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPositiopn();

	/// <summary>
	/// 衝突フラグ
	/// </summary>
	bool GetIsHit() { return isHit_; }

	bool GetRank() { return rank_; }

	bool GetToTitle() { return toTitle_; }

	// セッター関数
	/// <summary>
	/// 親子関係
	/// </summary>
	void SetParent(const WorldTransform* parent);

private:
	Audio* audio_ = nullptr;

	// 本体
	Model* model_ = nullptr;      // モデル
	uint32_t textureHandle_ = 0u; // テクスチャハンドル

	WorldTransform worldTransform_; // ワールド変換
	float radius_ = 1.0f;           // 半径

	bool isHit_ = false; // 衝突フラグ

	uint32_t soundGoal_ = 0u; // サウンドハンドル
	uint32_t voiceGoal_ = 0u; // 再生ハンドル

	// UI
	Sprite* spriteClear_ = nullptr;          // スプライト
	uint32_t textureHandleClear_ = 0u;  // テクスチャハンドル

	WorldTransform worldTransformClear_; // ワールド変換

	bool rank_ = false; // ランクフラグ

	Input* input_;

	bool toTitle_ = false; // タイトルへ

	int32_t triggerCount_ = 5; // 連続押し防止
};
