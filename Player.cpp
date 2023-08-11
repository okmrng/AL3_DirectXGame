﻿#include "Player.h"

Player::~Player() { 
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete spriteReticle_;
}

void Player::Initialize(Model* model) {
	// 自機
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("player/player.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った情報をセット
	worldTransform_.translation_ = {0.0f, 0.0f, 15.0f};

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// レティクル
	// レティクルのワールドトランスフォーム初期化
	worldTransformReticle_.Initialize();

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("player/target.png");
	// スプライト生成
	spriteReticle_ = Sprite::Create(textureReticle, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});
}

void Player::Update(ViewProjection& viewProjection) {
	// キャラクターの移動ベクトル
	Vector3 move{0, 0, 0};
	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向でベクトル移動
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// 範囲制限
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	// 範囲を越えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	// 行列更新
	worldTransform_.UpdateMatrix();
	// 行列の転送
	worldTransform_.TransferMatrix();

	// 回転速さ(ラジアン/frame)
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}

	// キャラクター攻撃処理
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->GetisDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// マウスカーソルのスクリーン座標からワールド座標を取得して2Dレティクル配置
	UpdateReticle(viewProjection);
}

void Player::UpdateReticle(ViewProjection& viewProjection) {
	POINT mousePosition;

	// マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2Dレティクルのスプライトに代入する
	spriteReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

	// ビューポート行列
	Matrix4x4 matViewport =
	    MakeViewportMatrix(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matViewProjectionViewport =
	    viewProjection.matView * viewProjection.matProjection * matViewport;

	// 合成行列の逆行列
	Matrix4x4 matInverseViewProjectionViewport = Inverse(matViewProjectionViewport);

	// スクリーン座標
	Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);
	Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseViewProjectionViewport);
	posFar = Transform(posFar, matInverseViewProjectionViewport);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;

	mouseDirection = Normalize(mouseDirection);

	// カメラから標準オブジェクトの距離
	const float kDistanceTestObject = 50.0f;
	worldTransformReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;

	// ワールド行列の更新
	worldTransformReticle_.UpdateMatrix();
	// ワールド行列の転送
	worldTransformReticle_.TransferMatrix();
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデル
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI() { 
	spriteReticle_->Draw();
}

void Player::Attack() { 
	if (input_->PushKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity;
		velocity.x = worldTransformReticle_.translation_.x - worldTransform_.matWorld_.m[3][0];
		velocity.y = worldTransformReticle_.translation_.y - worldTransform_.matWorld_.m[3][1];
		velocity.z = worldTransformReticle_.translation_.z - worldTransform_.matWorld_.m[3][2];

		// 正規化
		velocity = Normalize(velocity);

		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPositiopn(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::OnColision() {}

Vector3 Player::GetWorldPositiopn() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
