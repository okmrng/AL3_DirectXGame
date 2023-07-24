#pragma once

class Colider {
public:
	// 半径を取得
	float GetRadius() { return radius_; }

	// 半径を設定
	void SetRadius(float radius) { radius_ = radius; }

	// 衝突時に呼ばれる関数
	virtual void OnColision();

	// ワールド座標を取得
	virtual Vector3 GetWorldPosition() = 0;

private:
	// 衝突判定
	float radius_ = 50.0f;
};
