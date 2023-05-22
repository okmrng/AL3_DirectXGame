#include "Enemy.h"

#include <cassert>

void Enemy::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("fnf_freeeee.png");
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	//速度
	velocity_ = {-1, 1, -1};
}

void Enemy::Update() {
	

	worldTransform_.UpdateMatrix();
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Enemy::ApproachUpdate() {
	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_.z += velocity_.z;

	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate() {
	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
}
