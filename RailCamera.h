#pragma once

#include "WorldTransform.h"
#include "ViewProjection.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	// ゲッター関数
	const ViewProjection& GetViewProjection() { return viewProjection_; }
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	// セッター関数
	/// <summary>
	/// 平行移動
	/// </summary>
	/// <param name="move_"></param>
	void SetMove(Vector3 move);

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rot"></param>
	void SetRot(Vector3 rot);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 座標に加算する値
	Vector3 move_ = {0.0f, 0.0f, 0.0f}; // 平行移動
	Vector3 rot_ = {0.0f, 0.001f, 0.0f}; // 回転
};
