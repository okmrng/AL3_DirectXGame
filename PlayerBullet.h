#pragma once

#include "Model.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

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
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnColision();

	//ゲッター
	bool GetisDead() const { return isDead_; }

	/// <summary>
	/// 半径取得
	/// </summary>
	/// <returns></returns>
	const float GetRadius() { return radius_; }

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPositiopn();

private:
	WorldTransform worldTransform_;          // ワールド変換データ
									         
	// モデル						          
	Model* model_ = nullptr;		         
	uint32_t textureHandle_ = 0u;            // テクスチャハンドル
									         
	Vector3 velocity_;                       // 速度

	static const int32_t kLifeTime = 60 * 5; // 寿命

	// デス
	int32_t deathTimer_ = kLifeTime;         // デスタイマー
	bool isDead_ = false;                    // デスフラグ

	const float radius_ = 1.0f;              // 半径
};
