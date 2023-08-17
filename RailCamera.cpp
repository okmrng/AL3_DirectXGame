#include "RailCamera.h"
#include "Matrix4x4.h"
#include "ImGuiManager.h"

void RailCamera::Initialize() {
	// ワールド変換の初期化
	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update() { 
	// 座標の加算
	worldTransform_.translation_.x += move_.x;
	worldTransform_.translation_.y += move_.y;
	worldTransform_.translation_.z += move_.z;

	// 角度の加算
	worldTransform_.rotation_.x += rot_.x;
	worldTransform_.rotation_.y += rot_.y;
	worldTransform_.rotation_.z += rot_.z;

	// ワールド行列の再計算
	worldTransform_.UpdateMatrix();

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Inverse(worldTransform_.matWorld_);
	viewProjection_.TransferMatrix();
}

void RailCamera::SetData(Vector3 move, Vector3 rot) {
	move_ = move;
	rot_ = rot;
}
