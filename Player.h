﻿#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "Input.h"
#include "PlayerBullet.h"
#include "PlayerBomb.h"
#include <list>
#include "Sprite.h"
#include <cassert>
#include "ImGuiManager.h"
#include "MathUtility.h"

// 前方宣言
class RailCamera;

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
	/// /// <param name="viewProjection">ビュープロジェクション</param>
	void Update(ViewProjection& viewProjection);

	/// <summary>
	/// レティクル更新
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void UpdateReticle(ViewProjection& viewProjection);

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
	/// ボム
	/// </summary>
	void Bomb();

	/// <summary>
	/// ボムの当たった時の処理
	/// </summary>
	void OnBombCollision();

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
	/// ローカル座標取得
	/// </summary>
	Vector3 GetTranslation();

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	Vector3 GetBombWorldPositiopn();

	/// <summary>
	/// 半径
	/// </summary>
	const float GetRadius() { return radius_; }

	uint32_t GetBombTextureHandle();

	// セッター
	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	void SetRailCamera(RailCamera* railCamera) { railCamera_ = railCamera; }

	void SetMisalignment(Vector3 misalignment) { misalignment_ = misalignment; }

private:
	// 自機
	WorldTransform worldTransform_;            // ワールド変換データ

	Vector3 misalignment_ = {0.0f, 0.0f, 0.1f}; // カメラが動いている時のズレ修正
									           
	// モデル						            
	Model* model_ = nullptr;		           
	uint32_t textureHandle_ = 0u;              // テクスチャハンドル
									           
	Input* input_ = nullptr;                   // キーボード入力
											   
	// 弾							    	   
	std::list<PlayerBullet*> bullets_;         // 弾

	static const int32_t kAttackInterval_ = 8; // 発射間隔
	int32_t kAttackTimer_ = kAttackInterval_;  // 発射タイマー
											   
	// ボム									   
	PlayerBomb* bomb_ = nullptr;               // ボム	
											   
	bool canBomb_ = true;                      // ボムフラグ
	static const int32_t kCoolTime_ = 60 * 3;  // ボムのクールタイムの長さ
	int32_t bombTimer_ = kCoolTime_;           // ボムのクールタイム
											   
	const float radius_ = 1.0f;                // 半径
											   
	// 2Dレティクル							   
	WorldTransform worldTransformReticle_;     // ワールド変換データ
								
	Model* model3DReticle_ = nullptr;

	Sprite* spriteReticle_ = nullptr;          // スプライト

	RailCamera* railCamera_ = nullptr; // レールカメラ
};
