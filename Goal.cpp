#include "Goal.h"

void Goal::Initialize(Model* model) { 
	// モデルをセット
	assert(model); 
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("model/goal.png");

	// ワールド変換初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 25.0f, 50.0f};
}

void Goal::Update() {
	// 回転
	Vector3 rot = {0.05f, 0.01f, 0.05f};
	worldTransform_.rotation_ += rot;

	// 移動
	float move = -0.1f;
	if (worldTransform_.translation_.y <= 0.0f) {
		move = 0.0f;
	}
	worldTransform_.translation_.y += move;

	worldTransform_.UpdateMatrix();
}

void Goal::Draw(ViewProjection& viewProjection) {
	if (isHit_ == false) {
		model_->Draw(worldTransform_, viewProjection, textureHandle_);
	}
}

void Goal::OnColision() { isHit_ = true; }

Vector3 Goal::GetWorldPositiopn() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Goal::SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }
