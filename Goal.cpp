#include "Goal.h"

void Goal::Initialize(Model* model) { 
	// モデルをセット
	assert(model); 
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("model/goal.png");

	// ワールド変換初期化
	worldTransform_.Initialize();
}

void Goal::Update() {
	// 回転
	Vector3 rot = {1.0f, 1.0f, 1.0f};

	worldTransform_.rotation_ += rot;
}

void Goal::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
