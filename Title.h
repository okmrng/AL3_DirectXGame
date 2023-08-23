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
/// タイトル
/// </summary>
class Title {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Title();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Title();

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
	/// <summary>
	/// 次のシーンへ遷移するフラグ
	/// </summary>
	bool GetToNext() { return toNext_; }

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	uint32_t textureHandle_title_bg = 0u; // 背景のテクスチャハンドル

	Sprite* sprite_title_bg = nullptr;    // 背景のスプライト
									      
	bool toNext_ = false;                 // 次のシーンへ遷移するフラグ

	uint32_t soundDataHandle_ = 0u; // サウンドデータハンドル
};
