#include "Skydome.h"

void Skydome::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 2.0f, 0.0f};
}

void Skydome::Update() { 
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection);
}
