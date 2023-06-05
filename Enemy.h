#pragma once

#include "Model.h"
#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class Enemy {
public:
	//メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	void Initialize(Model* model, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// 接近フェーズの更新関数
	/// </summary>
	void ApproachUpdate();

	/// <summary>
	/// 離脱フェーズの更新関数
	/// </summary>
	void LeaveUpdate();

private:
	//メンバ関数ポインタのテーブル
	static void (Enemy::*spFuncTable[])();

	//メンバ変数
	//  ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// 速度
	Vector3 velocity_;

	//行動フェーズ
	enum class Phase {
		Approach, //接近する
		Leave     //離脱する
	};
	//フェーズ
	Phase phase_ = Phase::Approach;
};
