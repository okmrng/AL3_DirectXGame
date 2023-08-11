#pragma once

#include <Input.h>
#include <Sprite.h>
#include <TextureManager.h>

/// <summary>
/// タイトル
/// </summary>
class Title {
public:
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
	uint32_t textureHandle_title_bg = 0u; // 背景のテクスチャハンドル

	Sprite* sprite_title_bg = nullptr;    // 背景のスプライト
									      
	bool toNext_ = false;                 // 次のシーンへ遷移するフラグ
									      
	Input* input_ = nullptr;              // キー入力
};
