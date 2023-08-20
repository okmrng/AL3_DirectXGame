#include "Goal.h"

void Goal::Initialize(Model* model) { 
	// 本体
	// モデルをセット
	assert(model); 
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("model/goal.png");

	// ワールド変換初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 25.0f, 50.0f};

	// UI
	// ワールド変換初期化
	worldTransformClear_.Initialize();
	worldTransformClear_.translation_ = {1400.0f, 150.0f, 0.0f};

	// テクスチャ読み込み
	textureHandleClear_ = TextureManager::Load("sprite/clear.png");
	// スプライト生成
	spriteClear_ = Sprite::Create(
	    textureHandleClear_,
	    Vector2(worldTransformClear_.translation_.x, worldTransformClear_.translation_.y),
	    {1, 1, 1, 1}, {0.0f, 0.0f});
}

void Goal::Update() {
	// 本体
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

	// UI
	// 移動
	float moveClear = -200.0f;
	if (isHit_) {
		worldTransformClear_.translation_.x += moveClear;
	}
	if (worldTransformClear_.translation_.x <= 1.0f) {
		moveClear = 0.0f;
		worldTransformClear_.translation_.x = 1.0f;
	}
	spriteClear_->SetPosition(
	    Vector2(worldTransformClear_.translation_.x, worldTransformClear_.translation_.y));

	worldTransformClear_.UpdateMatrix();
	worldTransformClear_.TransferMatrix();
}

void Goal::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void Goal::DrawUI() { 
	spriteClear_->Draw();
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
