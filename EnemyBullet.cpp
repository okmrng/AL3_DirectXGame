#include "EnemyBullet.h"

#include <cassert>
#include <cmath>
#include <MathUtility.h>
#include "Player.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("black.jpg");
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	worldTransform_.scale_.x = 0.5f;
	worldTransform_.scale_.y = 0.5f;
	worldTransform_.scale_.z = 3.0f;
}

void EnemyBullet::Update() {
	assert(player_);

	Vector3 playerWorldPos = player_->GetWorldPositiopn(); // 自キャラのワールド座標
	Vector3 enemyBulletWorldPos = GetWorldPosition();      // 敵弾のワールド座標

	// 敵弾から自キャラへのベクトル
	Vector3 toPlayer = enemyBulletWorldPos - playerWorldPos;

	// ベクトルの正規化
	toPlayer = Normalize(toPlayer);
	velocity_ = Normalize(velocity_);

	velocity_ = Lerp(velocity_, toPlayer, 1) * 0.5f;

	// 回転を合わせる
	// (θy)
	worldTransform_.rotation_.y = std::atan2(velocity_.x, velocity_.z);

	// 横軸
	float velocityXZ = sqrt(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

	// (θx)
	worldTransform_.rotation_.x = std::atan2(-velocity_.y, velocityXZ);

	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	/*worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;*/

	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

Vector3 EnemyBullet::GetWorldPosition() { 
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}
