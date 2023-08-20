#include "Goal.h"

void Goal::Initialize(Model* model) { 
	// モデルをセット
	assert(model); 
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("model/goal.png");

	// ワールド変換初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 50.0f};
}

void Goal::Update() {
	// 回転
	Vector3 rot = {0.05f, 0.01f, 0.05f};

	worldTransform_.rotation_ += rot;

	worldTransform_.UpdateMatrix();
}

void Goal::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Goal::SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }
