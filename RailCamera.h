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

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPositiopn();

	// セッター関数
	/// <summary>
	/// 平行移動と回転をセットする
	/// </summary>
	/// <param name="move">平行移動</param>
	/// <param name="rot">回転</param>
	void SetData(Vector3 move, Vector3 rot);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 座標に加算する値
	Vector3 move_ = {0.0f, 0.0f, 0.1f}; // 平行移動
	Vector3 rot_ = {0.0f, 0.00f, 0.0f}; // 回転
};
