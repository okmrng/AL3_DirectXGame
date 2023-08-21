#pragma once

#include <WorldTransform.h>
#include <Model.h>
#include <cassert>

/// <summary>
/// 弾が当たった時のエフェクト
/// </summary>
class HitBullet {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 position, Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection& viewProjection);

	// ゲッター
	bool GetIsDead() const { return isDead_; }

	// セッター
	void SetMisalignment(Vector3 misalignment) { misalignment_ = misalignment; }

private:
	WorldTransform worldTransform_[8]; // ワールド変換
	Vector3 velocity_[8] = {
	    {-0.2f, 0.2f,  0.0f},
        {0.0f,  0.3f,  0.0f},
        {0.2f,  0.2f,  0.0f},
	    {0.3f,  0.0f,  0.0f},
        {0.2f,  -0.2f, 0.0f},
        {0.0f,  -0.3f, 0.0f},
	    {-0.2f, -0.2f, 0.0f},
        {-0.3f, 0.0f,  0.0f}
    }; // 速度
	Vector3 misalignment_; // カメラが動いている時のズレ修正

	Model* model_[8];             // モデル
	uint32_t textureHandle_ = 0u; // テクスチャハンドル

	int32_t count_ = 15;  // デスまでのカウント
	bool isDead_ = false; // デス
};
