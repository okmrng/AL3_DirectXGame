#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "DebugCamera.h"
#include "Enemy.h"
#include "EnemyIntervalShort.h"
#include "EnemyStrong.h"
#include "EnemyMove.h"
#include "EnemyBullet.h"
#include "Skydome.h"
#include "RailCamera.h"
#include "Goal.h"
#include "Score.h"
#include "HitBullet.h"
#include <sstream>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet">敵弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="enemy">敵</param>
	/// <param name="pos">座標</param>
	void AddEnemy(Vector3 pos, Vector3 velocity, Vector3 misalignment, int32_t toLeaveTimer);

	/// <summary>
	/// 発射間隔が短い敵の生成
	/// </summary>
	/// <param name="enemy">敵</param>
	/// <param name="pos">座標</param>
	void AddEnemyIntervalShort(
	    Vector3 pos, Vector3 velocity, Vector3 misalignment, int32_t toLeaveTimer);

	/// <summary>
	/// 強い敵の生成
	/// </summary>
	/// <param name="enemy">敵</param>
	/// <param name="pos">座標</param>
	void AddEnemyStrong(Vector3 pos, Vector3 velocity, Vector3 misalignment, int32_t toLeaveTimer);

	/// <summary>
	/// 動く敵の生成
	/// </summary>
	/// <param name="enemy">敵</param>
	/// <param name="pos">座標</param>
	void AddEnemyMove(
	    Vector3 pos, Vector3 velocity, const Vector3& leaveVelocity, Vector3 misalignment,
	    int32_t toLeaveTimer);


	void AddHitBullet(Vector3 pos, uint32_t textureHandle);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopComands();

	/// <summary>
	/// 発射間隔が短い敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyIntervalShortPopComands();

	/// <summary>
	/// 強い敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyStrongPopComands();

	/// <summary>
	/// 動く敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyMovePopComands();

	/// <summary>
	/// レールカメラデータの読み込み
	/// </summary>
	void LoadRailCamera();

	/// <summary>
	/// レールカメラのコマンドの更新
	/// </summary>
	void UpdateRailCameraComands();

	// ゲッター
	bool GetToTitle() { return toTitle_; }

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
	// 3Dモデル
	Model* model_ = nullptr;
	Model* modelSkydome_ = nullptr;

	ViewProjection viewProjection_; // ビュープロジェクション

	Player* player_ = nullptr; // 自機

	// デバッグカメラ
	DebugCamera* debugCamera_ = nullptr; // デバッグカメラ
	bool isDebugCameraActive_ = false;   // デバッグカメラ有効フラグ

	// 敵
	std::list<Enemy*> enemy_;                           // 敵
	std::list<EnemyIntervalShort*> enemyIntervalShort_; // 発射間隔が短い敵
	std::list<EnemyStrong*> enemyStrong_;               // 強い敵
	std::list<EnemyMove*> enemyMove_;                   // 動く敵

	std::list<EnemyBullet*> bullets_; // 敵弾

	Skydome* skydome_ = nullptr; // 天球

	RailCamera* railCamera_ = nullptr; // レールカメラ

	Goal* goal_ = nullptr; // ゴール

	Score* score_ = nullptr; // スコア

	std::list<HitBullet*> hitBullet_; // 弾が当たった時のエフェクト

	// コマンド
	std::stringstream enemyPopComands;              // 敵発生コマンド
	std::stringstream enemyIntervalShortPopComands; // 発射間隔が短い敵発生コマンド
	std::stringstream enemyStrongPopComands;        // 強い敵発生コマンド
	std::stringstream enemyMovePopComands;          // 動く敵発生コマンド
	std::stringstream railCameraComands;            // レールカメラのコマンド

	// 待機
	// 通常の敵
	bool enemyIsWait_;       // 待機中フラグ
	int32_t enemyWaitTimer_; // 待機タイマー
	// 発射間隔が短い敵
	bool enemyIntervalShortIsWait_;       // 待機中フラグ
	int32_t enemyIntervalShortWaitTimer_; // 待機タイマー
	// 強い敵
	bool enemyStrongIsWait_;       // 待機中フラグ
	int32_t enemyStrongWaitTimer_; // 待機タイマー
	// 動く敵
	bool enemyMoveIsWait_;       // 待機中フラグ
	int32_t enemyMoveWaitTimer_; // 待機タイマー
	// レールカメラ
	bool asItIs_; // 変化なしフラグ
	int32_t asItIsTimer_; // 次変化起こすまでの時間

	int32_t count_ = 0; // 13600; // ゲームカウント

	bool toTitle_ = false; // タイトルへ
};
