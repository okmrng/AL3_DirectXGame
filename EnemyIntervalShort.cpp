#include "EnemyIntervalShort.h"

#include "GameScene.h"
#include "Player.h"

void EnemyIntervalShort::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("model/enemy.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// 速度
	velocity_ = velocity;

	// 接近フェーズ初期化
	ApproachInitialize();
}

void EnemyIntervalShort::Update() {
	// フェーズ
	switch (phase_) {
	case Phase::Approach:
	default:
		// 移動
		ApproachUpdate();
		break;
	case Phase::Leave:
		LeaveUpdate();
		break;
	}

	// HPが0になったらデス
	if (HP <= 0) {
		isDead_ = true;
	}

	worldTransform_.UpdateMatrix();
}

void EnemyIntervalShort::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void EnemyIntervalShort::ApproachInitialize() {
	// 発射タイマーを初期化
	kFireTimer = kFireInterval;
}

void EnemyIntervalShort::ApproachUpdate() {
	// 座標を移動させる
	worldTransform_.translation_.z += velocity_.z;

	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 13.0f) {
		phase_ = Phase::Leave;
	}

	// 発射タイマーカウントダウン
	--kFireTimer;
	if (kFireTimer <= 0) {
		// 弾を発射
		Fire();
		// 発射タイマーを初期化
		kFireTimer = kFireInterval;
	}
}

void EnemyIntervalShort::LeaveUpdate() {
	if (deathTimer_-- < 0) {
		isDead_ = true;
	}

	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
}

void EnemyIntervalShort::Fire() {
	assert(player_);

	// 弾の速度
	const float kBulletSpeed = -0.5f;

	Vector3 playerWorldPos = player_->GetWorldPositiopn(); // 自キャラのワールド座標
	Vector3 enemyWorldPos = GetWorldPositiopn();           // 敵キャラのワールド座標
	// 敵キャラと自キャラの差分ベクトルを求める
	Vector3 velocity;
	velocity.x = enemyWorldPos.x - playerWorldPos.x;
	velocity.y = enemyWorldPos.y - playerWorldPos.y;
	velocity.z = enemyWorldPos.z - playerWorldPos.z;
	// 正規化
	velocity = Normalize(velocity);
	// 長さを速さに合わせる
	velocity.x *= kBulletSpeed;
	velocity.y *= kBulletSpeed;
	velocity.z *= kBulletSpeed;

	// 弾を生成し初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// 弾を登録する
	gameScene_->AddEnemyBullet(newBullet);
}

void EnemyIntervalShort::OnColision() { --HP; }

void EnemyIntervalShort::OnBombCollision() { isDead_ = true; }

Vector3 EnemyIntervalShort::GetWorldPositiopn() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}