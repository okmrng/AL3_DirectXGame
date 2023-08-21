#include "HitBullet.h"

void HitBullet::Initialize(Vector3 position, Model* model, uint32_t textureHandle) {
	// モデル
	assert(model);
	// テクスチャ読み込み
	textureHandle_ = textureHandle;

	for (int32_t i = 0; i < 8; i++) {
		model_[i] = model;

		// ワールド変換
		worldTransform_[i].Initialize();
		worldTransform_[i].translation_ = position;
		worldTransform_[i].scale_ = {0.1f, 0.1f, 0.1f};
	}
}

void HitBullet::Update() {
	// 移動
	for (int32_t i = 0; i < 8; i++) {
		velocity_[i] += misalignment_;
		worldTransform_[i].translation_ += velocity_[i];

		worldTransform_[i].UpdateMatrix();
	}

	// デス
	if (--count_ <= 0) {
		isDead_ = true;
	}
}

void HitBullet::Draw(ViewProjection& viewProjection) {
	for (int32_t i = 0; i < 8; i++) {
		model_[i]->Draw(worldTransform_[i], viewProjection, textureHandle_);
	}
}
