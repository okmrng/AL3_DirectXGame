﻿#pragma once

#include <Model.h>
#include <cassert>

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

	// ゲッター関数
	/// <summary>
	/// 半径
	/// </summary>
	float GetRadius() { return radius_; }

private:
	Model* model_ = nullptr;      // モデル
	uint32_t textureHandle_ = 0u; // テクスチャハンドル

	WorldTransform worldTransform_; // ワールド変換
	float radius_ = 1.0f;           // 半径

	bool isDead_ = false; // デスフラグ
};
