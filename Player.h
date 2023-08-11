#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "PlayerBullet.h"
#include <list>
#include "Sprite.h"

/// <summary>
/// 自キャラ
/// </summary>
class Player {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(ViewProjection& viewProjection);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnColision();

	//ゲッター
	/// <summary>
	/// 弾リストを取得
	/// </summary>
	const std::list<PlayerBullet*>& GetBullets() { return bullets_; }
	
	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetWorldPositiopn();

	/// <summary>
	/// 半径
	/// </summary>
	const float GetRadius() { return radius_; }

	// セッター
	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

private:
	// 自機
	WorldTransform worldTransform_;          // ワールド変換データ
									         
	// モデル						          
	Model* model_ = nullptr;		         
	uint32_t textureHandle_ = 0u;            // テクスチャハンドル
									         
	Input* input_ = nullptr;                 // キーボード入力
									         
	std::list<PlayerBullet*> bullets_;       // 弾
									         
	const float radius_ = 1.0f;              // 半径

	// 2Dレティクル
	WorldTransform worldTransform3DReticle_; // ワールド変換データ

	Model* model3DReticle_ = nullptr;

	Sprite* sprite2DReticle_ = nullptr;      // スプライト
};
