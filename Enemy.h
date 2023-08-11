﻿#pragma once

#include "Model.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "EnemyBullet.h"
#include <list>

// 前方宣言
class Player;
class GameScene;

/// <summary>
/// 敵
/// </summary>
class Enemy {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Enemy();

	//メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	void Initialize(Model* model, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 接近フェーズの初期化関数
	/// </summary>
	void ApproachInitialize();

	/// <summary>
	/// 接近フェーズの更新関数
	/// </summary>
	void ApproachUpdate();

	/// <summary>
	/// 離脱フェーズの更新関数
	/// </summary>
	void LeaveUpdate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Fire();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnColision();

	// セッター
	void SetPlayer(Player* player) { player_ = player; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// ゲッター
	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPositiopn();

	/// <summary>
	/// 半径取得
	/// </summary>
	/// <returns></returns>
	const float GetRadius() { return radius_; }

	bool GetIsDead() { return isDead_; }

	//メンバ変数
	static const int kFireInterval = 60; // 発射間隔

private:
	int32_t kFireTimer = 0;              // 発射タイマー

	WorldTransform worldTransform_;      // ワールド変換データ

	// モデル
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;        // テクスチャハンドル

	Vector3 velocity_;                   // 速度

	// フェーズ
	// 行動フェーズ
	enum class Phase {
		Approach,                        // 接近する
		Leave                            // 離脱する
	};
	Phase phase_ = Phase::Approach;

	Player* player_ = nullptr;           // 自キャラ

	const float radius_ = 1.0f;          // 半径

	GameScene* gameScene_ = nullptr;     // ゲームシーン

	// デス
	int32_t deathTimer_ = 300;           // デスタイマー
	bool isDead_ = false;                // デスフラグ
};
