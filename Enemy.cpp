#include "Enemy.h"

#include <cassert>

Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}

	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

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
	velocity_ = {-0.1f, 0.1f, -0.2f};

	//接近フェーズ初期化
	ApproachInitialize();
}

void Enemy::Update() {
	//フェーズ
	//switch (phase_) {
	//case Phase::Approach:
	//default:
	//	//移動(ベクトルを加算)
	//	ApproachUpdate();
	//	break;
	//case Phase::Leave:
	//	LeaveUpdate();
	//	break;
	//}

	worldTransform_.UpdateMatrix();

	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});

	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Enemy::ApproachInitialize() {
	//発射タイマーを初期化
	kFireTimer = kFireInterval;

	// 発射タイマーをセットする
	FireReset();
}

void Enemy::ApproachUpdate() {
	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_.z += velocity_.z;

	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}

	//FireReset();

	//発射タイマーカウントダウン
	//--kFireTimer;
	//if (kFireTimer <= 0) {
	//	//弾を発射
	//	Fire();
	//	//発射タイマーを初期化
	//	kFireTimer = kFireInterval;
	//}
}

void Enemy::LeaveUpdate() {
	// 座標を移動させる(1フレーム分の移動量を足しこむ)
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;

	timedCalls_.clear();
}

void Enemy::Fire() {
	Vector3 velocity(0, 0.1f, -1.0f);
	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_,velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);
}

void Enemy::FireReset() {
	Fire();

	// 発射タイマーのセット
	timedCalls_.push_back(new TimedCall(std::bind(&Enemy::FireReset, this), 60));
}
