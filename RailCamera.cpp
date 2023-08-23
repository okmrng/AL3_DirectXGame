#include "RailCamera.h"
#include "Matrix4x4.h"
#include "ImGuiManager.h"

void RailCamera::Initialize() {
	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};

	move_ = {0.0f, 0.0f, 0.1f};

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

	ImGui::Begin("RailCamera");
	ImGui::Text("velocityX:%.1f", move_.x);
	ImGui::Text("velocityY:%.1f", move_.y);
	ImGui::Text("velocityZ:%.1f", move_.z);
	ImGui::End();
}

Vector3 RailCamera::GetWorldPositiopn() { 
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void RailCamera::SetData(Vector3 move, Vector3 rot) {
	move_ = move;
	rot_ = rot;
}
