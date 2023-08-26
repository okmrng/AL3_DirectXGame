#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲーム説明
/// </summary>
class Explanation {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Explanation();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Explanation();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	// ゲッター
	bool GetToNext() { return toNext_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// 背景
	Sprite* sprite_ = nullptr;    // スプライト
	uint32_t textureHandle_ = 0u; // テクスチャハンドル

	// SPACE
	Sprite* spriteSpace_ = nullptr;    // スプライト
	uint32_t textureHandleSpace_ = 0u; // テクスチャハンドル

	bool toNext_ = false; // 次のシーンへ

	int32_t keyCount_ = 5; // キーカウント
};
