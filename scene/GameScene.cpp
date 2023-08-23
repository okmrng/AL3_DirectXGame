#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete modelSkydome_;
	delete player_;
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		delete enemyIntervalShort;
	}
	for (EnemyStrong* enemyStrong : enemyStrong_) {
		delete enemyStrong;
	}
	for (EnemyMove* enemyMove : enemyMove_) {
		delete enemyMove;
	}
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	delete skydome_;
	delete railCamera_;
	delete debugCamera_;
	delete goal_;
	delete score_;
	for (HitBullet* hitBullet : hitBullet_) {
		delete hitBullet;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 3Dモデル
	model_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// ビュープロジェクション
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_);

	// 敵発生
	// 通常の敵
	LoadData("Resources/script/enemyData.csv", enemyPopComands);

	// 発射間隔が短い敵
	LoadData("Resources/script/enemyIntervalShortData.csv", enemyIntervalShortPopComands);

	// 強い敵
	LoadData("Resources/script/enemyStrongData.csv", enemyStrongPopComands);

	// 動く敵
	LoadData("Resources/script/enemyMoveData.csv", enemyMovePopComands);

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	// レールカメラ
	railCamera_ = new RailCamera();
	railCamera_->Initialize();
	// 自キャラとの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());
	player_->SetRailCamera(railCamera_);
	for (EnemyBullet* bullet : bullets_) {
		bullet->Set(&railCamera_->GetWorldTransform());
	}
	
	LoadData("Resources/script/railCameraData.csv", railCameraComands);

	// ゴール
	goal_ = new Goal();
	goal_->Initialize(model_);
	// レールカメラと親子関係を結ぶ
	goal_->SetParent(&railCamera_->GetWorldTransform());

	// スコア
	score_ = new Score();
	score_->Initialize();

	count_ = 13600;

	enemyWaitTimer_ = 0;
	enemyIntervalShortWaitTimer_ = 0;
	enemyStrongWaitTimer_ = 0;
	enemyMoveWaitTimer_ = 0;
	asItIsTimer_ = 0;

	toTitle_ = false;

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);
}

void GameScene::Update() {
	if (goal_->GetIsHit() == false) {
		// カウント
		--count_;

		// 自キャラ
		player_->Update(viewProjection_);

		// 通常の敵
		UpdateEnemyPopComands();
		for (Enemy* enemy : enemy_) {
			enemy->Update();
		}

		// 倒したらスコア加算
		for (Enemy* enemy : enemy_) {
			if (enemy->GetIsHit()) {
				score_->AddScore50();
			}
		}

		// デスフラグの立った敵を削除
		enemy_.remove_if([](Enemy* enemy) {
			if (enemy->GetIsDead()) {
				delete enemy;
				return true;
			}
			return false;
		});

		// 発射間隔が短い敵
		UpdateEnemyIntervalShortPopComands();
		for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
			enemyIntervalShort->Update();
		}

		// 倒したらスコア加算
		for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
			if (enemyIntervalShort->GetIsHit()) {
				score_->AddScore50();
			}
		}

		// デスフラグの立った敵を削除
		enemyIntervalShort_.remove_if([](EnemyIntervalShort* enemyIntervalShort) {
			if (enemyIntervalShort->GetIsDead()) {
				delete enemyIntervalShort;
				return true;
			}
			return false;
		});

		// 強い敵
		UpdateEnemyStrongPopComands();
		for (EnemyStrong* enemyStrong : enemyStrong_) {
			enemyStrong->Update();
		}

		// 倒したらスコア加算
		for (EnemyStrong* enemyStrong : enemyStrong_) {
			if (enemyStrong->GetIsHit()) {
				score_->AddScore300();
			}
		}

		// デスフラグの立った敵を削除
		enemyStrong_.remove_if([](EnemyStrong* enemyStrong) {
			if (enemyStrong->GetIsDead()) {
				delete enemyStrong;
				return true;
			}
			return false;
		});

		// 動く敵
		UpdateEnemyMovePopComands();
		for (EnemyMove* enemyMove : enemyMove_) {
			enemyMove->Update();
		}

		// 倒したらスコア加算
		for (EnemyMove* enemyMove : enemyMove_) {
			if (enemyMove->GetIsHit()) {
				score_->AddScore100();
			}
		}

		// デスフラグの立った敵を削除
		enemyMove_.remove_if([](EnemyMove* enemyMove) {
			if (enemyMove->GetIsDead()) {
				delete enemyMove;
				return true;
			}
			return false;
		});

		// 敵弾更新
		for (EnemyBullet* bullet : bullets_) {
			bullet->Update();
		}

		// デスフラグの立った敵弾を削除
		bullets_.remove_if([](EnemyBullet* bullet) {
			if (bullet->GetisDead()) {
				delete bullet;
				return true;
			}
			return false;
		});

		// 当たり判定
		CheckAllCollisions();

		// 天球の更新
		skydome_->Update();

		// スコアの更新
		score_->Update();

		// 弾が当たった時のエフェクトの更新
		for (HitBullet* hitBullet : hitBullet_) {
			hitBullet->Update();
		}

		hitBullet_.remove_if([](HitBullet* hitBullet) {
			if (hitBullet->GetIsDead()) {
				delete hitBullet;
				return true;
			}
			return false;
		});

		// レールカメラの更新
		UpdateRailCameraComands();
		railCamera_->Update();
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
	}
	if (count_ <= 0) {
		// ゴール
		goal_->Update();

		// タイトルへ
		if (goal_->GetToTitle()) {
			toTitle_ = true;
		}
	}

// デバッグカメラの更新
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_C)) {
		isDebugCameraActive_ = true;
	}
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;

		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else if (!isDebugCameraActive_) {
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}

	ImGui::Begin("waitTimer");
	ImGui::Text("enemy             :%d", enemyWaitTimer_);
	ImGui::Text("enemyIntervalShort:%d", enemyIntervalShortWaitTimer_);
	ImGui::Text("enemyStrong       :%d", enemyStrongWaitTimer_);
	ImGui::Text("enemyMove         :%d", enemyMoveWaitTimer_);
	ImGui::Text("railCamera        :%d", asItIsTimer_);
	ImGui::End();
}

void GameScene::CheckAllCollisions() { 
	// 判定対象BとAの座標
	Vector3 posA, posB;

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

	// 自キャラと敵弾の当たり判定
	#pragma region
	// 自キャラの座標
	posA = player_->GetWorldPositiopn();

	// 自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : bullets_) {
		// 敵弾の座標
		posB = bullet->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = bullet->GetRadius();
		const float collisionRadiusAB =(collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnColision();
			// スコア
			score_->SubtractScore();
			// 弾が当たった時のエフェクト
			AddHitBullet(bullet->GetWorldPositiopn(), bullet->GetTextureHandle());
		}
	}
	#pragma endregion

	// 自キャラと敵キャラの当たり判定
	#pragma region
	// 自キャラの座標
	posA = player_->GetWorldPositiopn();

	for (Enemy* enemy : enemy_) {
		posB = enemy->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemy->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// スコア
			score_->SubtractScore();
		}
	}
	#pragma endregion

	// 自弾と敵キャラの当たり判定
	#pragma region
	for (Enemy* enemy : enemy_) {
		for (PlayerBullet* bullet : playerBullets) {
			// 敵キャラの座標
			posA = enemy->GetWorldPositiopn();
			// 自弾の座標
			posB = bullet->GetWorldPositiopn();

			// AとBの距離を求める
			const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
			const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
			const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
			const float collisionXYZ = collisionX + collisionY + collisionZ;

			const float collisionRadiusA = enemy->GetRadius();
			const float collisionRadiusB = bullet->GetRadius();
			const float collisionRadiusAB =
			    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

			// 球と球の交差判定
			if (collisionXYZ <= collisionRadiusAB) {
				// 敵キャラの衝突時コールバックを呼び出す
				enemy->OnColision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnColision();
				// スコア加算
				score_->AddScore();
				// 弾が当たった時のエフェクト
				AddHitBullet(bullet->GetWorldPositiopn(), bullet->GetTextureHandle());
			}
		}
	}
	#pragma endregion

	// ボムと敵キャラの当たり判定
	#pragma region
	// ボムの座標
	posA = player_->GetBombWorldPositiopn();

	for (Enemy* enemy : enemy_) {
		posB = enemy->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemy->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 敵キャラの衝突時コールバックを呼び出す
			enemy->OnColision();
			// ボムの衝突時コールバックを呼び出す
			player_->OnBombCollision();
			// スコア
			score_->AddScore50();
			// 弾が当たった時のエフェクト
			AddHitBullet(player_->GetBombWorldPositiopn(), player_->GetBombTextureHandle());
		}
	}
	#pragma endregion

	// 発射間隔が短い敵
	// 自キャラと敵キャラの当たり判定
	#pragma region
	// 自キャラの座標
	posA = player_->GetWorldPositiopn();

	for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		posB = enemyIntervalShort->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemyIntervalShort->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// スコア
			score_->SubtractScore();
		}
	}
	#pragma endregion

	// 自弾と敵キャラの当たり判定
	#pragma region
	for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		for (PlayerBullet* bullet : playerBullets) {
			// 敵キャラの座標
			posA = enemyIntervalShort->GetWorldPositiopn();
			// 自弾の座標
			posB = bullet->GetWorldPositiopn();

			// AとBの距離を求める
			const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
			const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
			const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
			const float collisionXYZ = collisionX + collisionY + collisionZ;

			const float collisionRadiusA = enemyIntervalShort->GetRadius();
			const float collisionRadiusB = bullet->GetRadius();
			const float collisionRadiusAB =
			    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

			// 球と球の交差判定
			if (collisionXYZ <= collisionRadiusAB) {
				// 敵キャラの衝突時コールバックを呼び出す
				enemyIntervalShort->OnColision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnColision();
				// スコア加算
				score_->AddScore();
				// 弾が当たった時のエフェクト
				AddHitBullet(bullet->GetWorldPositiopn(), bullet->GetTextureHandle());
			}
		}
	}
	#pragma endregion

	// ボムと敵キャラの当たり判定
	#pragma region
	// ボムの座標
	posA = player_->GetBombWorldPositiopn();

	for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		posB = enemyIntervalShort->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemyIntervalShort->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 敵キャラの衝突時コールバックを呼び出す
			enemyIntervalShort->OnColision();
			// ボムの衝突時コールバックを呼び出す
			player_->OnBombCollision();
			// スコア
			score_->AddScore50();
			// 弾が当たった時のエフェクト
			AddHitBullet(player_->GetBombWorldPositiopn(), player_->GetBombTextureHandle());
		}
	}
	#pragma endregion

	// 強い敵
	// 自キャラと敵キャラの当たり判定
	#pragma region
	// 自キャラの座標
	posA = player_->GetWorldPositiopn();

	for (EnemyStrong* enemyStrong : enemyStrong_) {
		posB = enemyStrong->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemyStrong->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// スコア
			score_->SubtractScore();
		}
	}
	#pragma endregion

	// 自弾と敵キャラの当たり判定
	#pragma region
	for (EnemyStrong* enemyStrong : enemyStrong_) {
		for (PlayerBullet* bullet : playerBullets) {
			// 敵キャラの座標
			posA = enemyStrong->GetWorldPositiopn();
			// 自弾の座標
			posB = bullet->GetWorldPositiopn();

			// AとBの距離を求める
			const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
			const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
			const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
			const float collisionXYZ = collisionX + collisionY + collisionZ;

			const float collisionRadiusA = enemyStrong->GetRadius();
			const float collisionRadiusB = bullet->GetRadius();
			const float collisionRadiusAB =
			    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

			// 球と球の交差判定
			if (collisionXYZ <= collisionRadiusAB) {
				// 敵キャラの衝突時コールバックを呼び出す
				enemyStrong->OnColision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnColision();
				// スコア加算
				score_->AddScore100();
				// 弾が当たった時のエフェクト
				AddHitBullet(bullet->GetWorldPositiopn(), bullet->GetTextureHandle());
			}
		}
	}
	#pragma endregion

	// ボムと敵キャラの当たり判定
	#pragma region
	// ボムの座標
	posA = player_->GetBombWorldPositiopn();

	for (EnemyStrong* enemyStrong : enemyStrong_) {
		posB = enemyStrong->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemyStrong->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 自キャラの衝突時コールバックを呼び出す
			enemyStrong->OnColision();
			// ボムの衝突時コールバックを呼び出す
			player_->OnBombCollision();
			// スコア
			score_->AddScore300();
			// 弾が当たった時のエフェクト
			AddHitBullet(player_->GetBombWorldPositiopn(), player_->GetBombTextureHandle());
		}
	}
	#pragma endregion

	// 動く敵
	// 自キャラと敵キャラの当たり判定
	#pragma region
	// 自キャラの座標
	posA = player_->GetWorldPositiopn();

	for (EnemyMove* enemyMove : enemyMove_) {
		posB = enemyMove->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemyMove->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// スコア
			score_->SubtractScore();
		}
	}
	#pragma endregion

	// 自弾と敵キャラの当たり判定
	#pragma region
	for (EnemyMove* enemyMove : enemyMove_) {
		for (PlayerBullet* bullet : playerBullets) {
			// 敵キャラの座標
			posA = enemyMove->GetWorldPositiopn();
			// 自弾の座標
			posB = bullet->GetWorldPositiopn();

			// AとBの距離を求める
			const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
			const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
			const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
			const float collisionXYZ = collisionX + collisionY + collisionZ;

			const float collisionRadiusA = enemyMove->GetRadius();
			const float collisionRadiusB = bullet->GetRadius();
			const float collisionRadiusAB =
			    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

			// 球と球の交差判定
			if (collisionXYZ <= collisionRadiusAB) {
				// 敵キャラの衝突時コールバックを呼び出す
				enemyMove->OnColision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnColision();
				// スコア加算
				score_->AddScore50();
				// 弾が当たった時のエフェクト
				AddHitBullet(bullet->GetWorldPositiopn(), bullet->GetTextureHandle());
			}
		}
	}
	#pragma endregion

	// ボムと敵キャラの当たり判定
	#pragma region
	// ボムの座標
	posA = player_->GetBombWorldPositiopn();

	for (EnemyMove* enemyMove : enemyMove_) {
		posB = enemyMove->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = player_->GetRadius();
		const float collisionRadiusB = enemyMove->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 敵キャラの衝突時コールバックを呼び出す
			enemyMove->OnColision();
			// ボムの衝突時コールバックを呼び出す
			player_->OnBombCollision();
			// スコア加算
			score_->AddScore100();
			// 弾が当たった時のエフェクト
			AddHitBullet(player_->GetBombWorldPositiopn(), player_->GetBombTextureHandle());
		}
	}
	#pragma endregion

	// 自弾とゴールの当たり判定
	#pragma region
	for (PlayerBullet* bullet : playerBullets) {
		// ゴールの座標
		posA = goal_->GetWorldPositiopn();
		// 自弾の座標
		posB = bullet->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = goal_->GetRadius();
		const float collisionRadiusB = bullet->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 敵キャラの衝突時コールバックを呼び出す
			goal_->OnColision();
			// 自弾の衝突時コールバックを呼び出す
			bullet->OnColision();
		}
	}
	#pragma endregion
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	// 自キャラの描画
	player_->Draw(viewProjection_);

	// ゴール
	if (goal_->GetIsHit() == false) {
		goal_->Draw(viewProjection_);
	}

	// 通常の敵の描画
	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}

	// 発射間隔が短い敵の描画
	for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		enemyIntervalShort->Draw(viewProjection_);
	}

	// 強い敵
	for (EnemyStrong* enemyStrong : enemyStrong_) {
		enemyStrong->Draw(viewProjection_);
	}

	// 動く敵
	for (EnemyMove* enemyMove : enemyMove_) {
		enemyMove->Draw(viewProjection_);
	}

	// 敵の弾の描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}

	// 弾が当たった時のエフェクトの描画
	for (HitBullet* hitBullet : hitBullet_) {
		hitBullet->Draw(viewProjection_);
	}

	// 天球の描画
	skydome_->Draw(viewProjection_);
	
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	// 自機
	player_->DrawUI();

	// ゴール
	if (count_ <= 0) {
		if (goal_->GetIsHit()) {
			goal_->DrawUI();
		}
	}

	// スコア
	score_->DrawUI(goal_->GetDrawHighScore());

	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	bullets_.push_back(enemyBullet);
}

void GameScene::AddEnemy(
    Vector3 pos, Vector3 velocity, Vector3 misalignment, int32_t toLeaveTimer) {
	// 生成
	Enemy* obj = new Enemy();
	// 初期化
	obj->Initialize(model_, pos + railCamera_->GetWorldPositiopn(), velocity, misalignment, toLeaveTimer);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);
	// レールカメラを渡す
	obj->SetRailCamera(railCamera_);

	enemy_.push_back(obj);
}

void GameScene::AddEnemyIntervalShort(
    Vector3 pos, Vector3 velocity, Vector3 misalignment, int32_t toLeaveTimer) {
	// 生成
	EnemyIntervalShort* obj = new EnemyIntervalShort();
	// 初期化
	obj->Initialize(model_, pos + railCamera_->GetWorldPositiopn(), velocity, misalignment, toLeaveTimer);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);
	// レールカメラを渡す
	obj->SetRailCamera(railCamera_);

	enemyIntervalShort_.push_back(obj);
}

void GameScene::AddEnemyStrong(
    Vector3 pos, Vector3 velocity, Vector3 misalignment, int32_t toLeaveTimer) {
	// 生成
	EnemyStrong* obj = new EnemyStrong();
	// 初期化
	obj->Initialize(model_, pos, velocity, misalignment, toLeaveTimer);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);
	// レールカメラと親子関係を結ぶ
	obj->SetParent(&railCamera_->GetWorldTransform());
	obj->SetRailCamera(railCamera_);

	enemyStrong_.push_back(obj);
}

void GameScene::AddEnemyMove(
    Vector3 pos, Vector3 velocity, const Vector3& leaveVelocity, Vector3 misalignment,
    int32_t toLeaveTimer) {
	// 生成
	EnemyMove* obj = new EnemyMove();
	// 初期化
	obj->Initialize(
	    model_, pos, velocity, leaveVelocity, misalignment,
	    toLeaveTimer);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);
	// レールカメラを渡す
	obj->SetRailCamera(railCamera_);
	// レールカメラと親子関係を結ぶ
	obj->SetParent(&railCamera_->GetWorldTransform());

	enemyMove_.push_back(obj);
}

void GameScene::AddHitBullet(Vector3 pos, uint32_t textureHandle) {
	// 生成
	HitBullet* obj = new HitBullet();
	// 初期化
	obj->Initialize(pos, model_, textureHandle);

	hitBullet_.push_back(obj);
}

void GameScene::LoadData(const std::string& filename, std::stringstream& targetStream) {
	std::ifstream file;
	// ファイルを開く
	file.open(filename);
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	// 文字列ストリームをクリア
	targetStream.str("");
	targetStream << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// ファイルを再度開いてポインタを先頭に戻す
	file.open(filename);
	assert(file.is_open());

	// エラーフラグをクリア
	file.clear();
	// ファイルポインタを先頭に戻す
	file.seekg(0, std::ios::beg); 
	// 再度ファイルを閉じる
	file.close();                 
}

void GameScene::UpdateEnemyPopComands() {
	// 待機処理
	if (enemyIsWait_ == true) {
		enemyWaitTimer_--;
		if (enemyWaitTimer_ <= 0) {
			// 待機完了
			enemyIsWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopComands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// 座標
			// x
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 速度
			// x
			getline(line_stream, word, ',');
			float velocityX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float velocityY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float velocityZ = (float)std::atof(word.c_str());
			// 弾のズレ修正
			// x
			getline(line_stream, word, ',');
			float misalignmentX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float misalignmentY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float misalignmentZ = (float)std::atof(word.c_str());

			// 離脱までの時間
			getline(line_stream, word, ',');
			int32_t toLeaveTime = atoi(word.c_str());

			// 敵を発生させる
			AddEnemy(
			    Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ),
			    Vector3(misalignmentX, misalignmentY, misalignmentZ), toLeaveTime);
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime_ = atoi(word.c_str());

			// 待機開始
			enemyIsWait_ = true;
			enemyWaitTimer_ = waitTime_;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyIntervalShortPopComands() {
	// 待機処理
	if (enemyIntervalShortIsWait_ == true) {
		enemyIntervalShortWaitTimer_--;
		if (enemyIntervalShortWaitTimer_ <= 0) {
			// 待機完了
			enemyIntervalShortIsWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyIntervalShortPopComands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// 座標
			// x
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 速度
			// x
			getline(line_stream, word, ',');
			float velocityX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float velocityY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float velocityZ = (float)std::atof(word.c_str());

			// 弾のズレ修正
			// x
			getline(line_stream, word, ',');
			float misalignmentX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float misalignmentY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float misalignmentZ = (float)std::atof(word.c_str());

			// 離脱までの時間
			getline(line_stream, word, ',');
			int32_t toLeaveTime = atoi(word.c_str());

			// 敵を発生させる
			AddEnemyIntervalShort(
			    Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ),
			    Vector3(misalignmentX, misalignmentY, misalignmentZ), toLeaveTime);
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime_ = atoi(word.c_str());

			// 待機開始
			enemyIntervalShortIsWait_ = true;
			enemyIntervalShortWaitTimer_ = waitTime_;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyStrongPopComands() {
	// 待機処理
	if (enemyStrongIsWait_ == true) {
		enemyStrongWaitTimer_--;
		if (enemyStrongWaitTimer_ <= 0) {
			// 待機完了
			enemyStrongIsWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyStrongPopComands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// 座標
			// x
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 速度
			// x
			getline(line_stream, word, ',');
			float velocityX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float velocityY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float velocityZ = (float)std::atof(word.c_str());

			// 弾のズレ修正
			// x
			getline(line_stream, word, ',');
			float misalignmentX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float misalignmentY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float misalignmentZ = (float)std::atof(word.c_str());

			// 離脱までの時間
			getline(line_stream, word, ',');
			int32_t toLeaveTime = atoi(word.c_str());

			// 敵を発生させる
			AddEnemyStrong(
			    Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ),
			    Vector3(misalignmentX, misalignmentY, misalignmentZ), toLeaveTime);
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			enemyStrongIsWait_ = true;
			enemyStrongWaitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyMovePopComands() {
	// 待機処理
	if (enemyMoveIsWait_ == true) {
		enemyMoveWaitTimer_--;
		if (enemyMoveWaitTimer_ <= 0) {
			// 待機完了
			enemyMoveIsWait_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyMovePopComands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// 座標
			// x
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 速度
			// x
			getline(line_stream, word, ',');
			float velocityX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float velocityY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float velocityZ = (float)std::atof(word.c_str());

			// 離脱時の速度
			// x
			getline(line_stream, word, ',');
			float leaveVelocityX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float leaveVelocityY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float leaveVelocityZ = (float)std::atof(word.c_str());

			// 弾のズレ修正
			// x
			getline(line_stream, word, ',');
			float misalignmentX = (float)std::atof(word.c_str());

			// y
			getline(line_stream, word, ',');
			float misalignmentY = (float)std::atof(word.c_str());

			// z
			getline(line_stream, word, ',');
			float misalignmentZ = (float)std::atof(word.c_str());

			// 離脱までの時間
			getline(line_stream, word, ',');
			int32_t toLeaveTime = atoi(word.c_str());

			// 敵を発生させる
			AddEnemyMove(
			    Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ),
			    Vector3(leaveVelocityX, leaveVelocityY, leaveVelocityZ),
			    Vector3(misalignmentX, misalignmentY, misalignmentZ), toLeaveTime);
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			enemyMoveIsWait_ = true;
			enemyMoveWaitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateRailCameraComands() {
	// 待機処理
	if (asItIs_ == true) {
		asItIsTimer_--;
		if (asItIsTimer_ <= 0) {
			// 待機完了
			asItIs_ = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(railCameraComands, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// CHANGEコマンド
		if (word.find("CHANGE") == 0) {
			// 平行移動
			// x座標
			getline(line_stream, word, ',');
			float moveX = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float moveY = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float moveZ = (float)std::atof(word.c_str());

			// 回転
			// x座標
			getline(line_stream, word, ',');
			float rotX = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float rotY = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float rotZ = (float)std::atof(word.c_str());

			// データをセット
			railCamera_->SetData(Vector3(moveX, moveY, moveZ), Vector3(rotX, rotY, rotZ));

			player_->SetMisalignment(Vector3(moveX, moveY, moveZ));
			for (Enemy* enemy : enemy_) {
				enemy->SetMisalignment(Vector3(moveX, moveY, moveZ));
			}
			for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
				enemyIntervalShort->SetMisalignment(Vector3(moveX, moveY, moveZ));
			}
			for (EnemyStrong* enemyStrong : enemyStrong_) {
				enemyStrong->SetMisalignment(Vector3(moveX, moveY, moveZ));
			}
			for (EnemyMove* enemyMove : enemyMove_) {
				enemyMove->SetMisalignment(Vector3(moveX, moveY, moveZ));
			}
			for (HitBullet* hitBullet : hitBullet_) {
				hitBullet->SetMisalignment(Vector3(moveX, moveY, moveZ));
			}
		}
		// ASITISコマンド
		else if (word.find("ASITIS") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t asItIsTime = atoi(word.c_str());

			// 待機開始
			asItIs_ = true;
			asItIsTimer_ = asItIsTime;

			// コマンドループを抜ける
			break;
		}
	}
}
