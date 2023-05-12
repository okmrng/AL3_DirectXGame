﻿#include "Player.h"

#include <cassert>
#include "ImGuiManager.h"

Player::~Player() {
	
}

void Player::Initialize(Model* model, uint32_t textureHandle) {
	//NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	//ワールド変換の初期化
	worldTransform_.Initialize();

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Player::Update() {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//キャラクターの移動ベクトル
	Vector3 move{0, 0, 0};

	//キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向でベクトル移動(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	//押した方向でベクトル移動(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	//範囲制限
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	//範囲を越えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	//行列更新
	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;

	worldTransform_.matWorld_ = MakeAffinMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

	//回転速さ(ラジアン/frame)
	const float kRotSpeed = 0.02f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}

	//キャラクター攻撃処理
	Attack();

	//弾更新
	//if (bullet_!=nullptr)と同じ条件
	if (bullet_) {
		bullet_->Update();
	}

	//キャラクターの座標を表示
	ImGui::SetNextWindowPos({60, 60});
	ImGui::SetNextWindowSize({300, 100});

	ImGui::Begin("Player");
	float sliderValue[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	ImGui::SliderFloat3("position", sliderValue, -20.0f, 20.0f);
	worldTransform_.translation_ = {sliderValue[0], sliderValue[1], sliderValue[2]};
	ImGui::End();
}

void Player::Draw(ViewProjection& viewProjection) {
	//3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//弾描画
	if (bullet_) {
		bullet_->Draw(viewProjection);
	}
}

void Player::Attack() { 
	if (input_->TriggerKey(DIK_SPACE)) {
		//弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_);

		//弾を登録する
		bullet_ = newBullet;
	}
}