#pragma once

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
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPositiopn();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnColision();

	// セッター
	void SetPlayer(Player* player) { player_ = player; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// ゲッター
	/// <summary>
	/// 弾リストを取得
	/// </summary>
	/// <returns></returns>
	//const std::list<EnemyBullet*>& GetBullets() { return bullets_; }

	/// <summary>
	/// 半径取得
	/// </summary>
	/// <returns></returns>
	const float GetRadius() { return radius_; }

	bool GetIsDead() { return isDead_; }

	//メンバ変数
	// 発射間隔
	static const int kFireInterval = 60;

private:
	//発射タイマー
	int32_t kFireTimer = 0;

	//  ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 速度
	Vector3 velocity_;

	//行動フェーズ
	enum class Phase {
		Approach, //接近する
		Leave     //離脱する
	};
	//フェーズ
	Phase phase_ = Phase::Approach;

	// 弾
	//std::list<EnemyBullet*> bullets_;

	//自キャラ
	Player* player_ = nullptr;

	//半径
	const float radius_ = 1.0f;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	// デスタイマー
	int32_t deathTimer_ = 300;
	// デスフラグ
	bool isDead_ = false;
};
