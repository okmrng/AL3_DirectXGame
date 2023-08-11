#pragma once

#include "WorldTransform.h"
#include "ViewProjection.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera {
public:
	// メンバ関数
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

private:
	// メンバ変数
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;
};
