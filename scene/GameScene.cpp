#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"
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
	delete title_;
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
	LoadEnemyPopData();

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_);

	// レールカメラ
	railCamera_ = new RailCamera();
	railCamera_->Initialize();
	// 自キャラとの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// タイトル
	title_ = new Title();
	title_->Initialize();

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向が参照するビュープロジェクションを参照する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	// タイトル
	if (scene_ == Scene::TITLE) {
		title_->Update();
		// タイトルからの遷移
		// メインゲームへ
		if (title_->GetToNext()) {
			scene_ = Scene::MAINGAME;
		}
	}

	if (scene_ == Scene::MAINGAME) {
		// 自キャラ
		player_->Update(viewProjection_);

		// ゲームオーバー
		/*if (player_->GetIsDead()) {
			scene_ = Scene::TITLE;
		}*/

		// 通常の敵
		//UpdateEnemyPopComands();
		//for (Enemy* enemy : enemy_) {
		//	enemy->Update();
		//}

		//// デスフラグの立った敵を削除
		//enemy_.remove_if([](Enemy* enemy) {
		//	if (enemy->GetIsDead()) {
		//		delete enemy;
		//		return true;
		//	}
		//	return false;
		//});

		//// 発射間隔が短い敵
		//UpdateEnemyIntervalShortPopComands();
		//for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		//	enemyIntervalShort->Update();
		//}

		//// デスフラグの立った敵を削除
		//enemyIntervalShort_.remove_if([](EnemyIntervalShort* enemyIntervalShort) {
		//	if (enemyIntervalShort->GetIsDead()) {
		//		delete enemyIntervalShort;
		//		return true;
		//	}
		//	return false;
		//});

		// 強い敵
		//UpdateEnemyStrongPopComands();
		//for (EnemyStrong* enemyStrong : enemyStrong_) {
		//	enemyStrong->Update();
		//}

		//// デスフラグの立った敵を削除
		//enemyStrong_.remove_if([](EnemyStrong* enemyStrong) {
		//	if (enemyStrong->GetIsDead()) {
		//		delete enemyStrong;
		//		return true;
		//	}
		//	return false;
		//});

		// 動く敵
		UpdateEnemyMovePopComands();
		for (EnemyMove* enemyMove : enemyMove_) {
			enemyMove->Update();
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

		// レールカメラの更新
		railCamera_->Update();
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;

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
	}
	
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
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnColision();
			// 敵弾の衝突時コールバックを呼び出す
			bullet->OnColision();
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
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnColision();
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
			// 敵の衝突時コールバックを呼び出す
			enemy->OnBombCollision();
		}
	}
	#pragma endregion

	// 自弾と敵弾の当たり判定
	#pragma region

	// 自弾と敵弾全ての当たり判定
	for (PlayerBullet* pBullet : playerBullets) {
		for (EnemyBullet* eBullet : bullets_) {
			// 自弾の座標
			posA = pBullet->GetWorldPositiopn();
			// 敵弾の座標
			posB = eBullet->GetWorldPositiopn();

			// AとBの距離を求める
			const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
			const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
			const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
			const float collisionXYZ = collisionX + collisionY + collisionZ;

			const float collisionRadiusA = pBullet->GetRadius();
			const float collisionRadiusB = eBullet->GetRadius();
			const float collisionRadiusAB =
			    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

			// 球と球の交差判定
			if (collisionXYZ <= collisionRadiusAB) {
				// 自キャラの衝突時コールバックを呼び出す
				//enemy_->OnColision();
				// 敵弾の衝突時コールバックを呼び出す
				eBullet->OnColision();
			}
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
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnColision();
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
			// 敵の衝突時コールバックを呼び出す
			enemyIntervalShort->OnBombCollision();
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
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnColision();
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
			// 自キャラの衝突時コールバックを呼び出す
			player_->OnColision();
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
			// 敵の衝突時コールバックを呼び出す
			enemyMove->OnBombCollision();
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
	// タイトル
	if (scene_ == Scene::TITLE) {
		title_->Draw();
	}
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
	// メインゲーム
	if (scene_ == Scene::MAINGAME) {
		// 自キャラの描画
		player_->Draw(viewProjection_);

		// 通常の敵の描画
		//for (Enemy* enemy : enemy_) {
		//	enemy->Draw(viewProjection_);
		//}

		//// 発射間隔が短い敵の描画
		//for (EnemyIntervalShort* enemyIntervalShort : enemyIntervalShort_) {
		//	enemyIntervalShort->Draw(viewProjection_);
		//}

		// 強い敵
		/*for (EnemyStrong* enemyStrong : enemyStrong_) {
			enemyStrong->Draw(viewProjection_);
		}*/

		// 動く敵
		for (EnemyMove* enemyMove : enemyMove_) {
			enemyMove->Draw(viewProjection_);
		}

		// 敵の弾の描画
		for (EnemyBullet* bullet : bullets_) {
			bullet->Draw(viewProjection_);
		}

		// 天球の描画
		skydome_->Draw(viewProjection_);
	}
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	// メインゲーム
	if (scene_ == Scene::MAINGAME) {
		player_->DrawUI();
	}
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	bullets_.push_back(enemyBullet);
}

void GameScene::AddEnemy(Vector3 pos, Vector3 velocity) {
	// 生成
	Enemy* obj = new Enemy();
	// 初期化
	obj->Initialize(model_, pos, velocity);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);

	enemy_.push_back(obj);
}

void GameScene::AddEnemyIntervalShort(Vector3 pos, Vector3 velocity) {
	// 生成
	EnemyIntervalShort* obj = new EnemyIntervalShort();
	// 初期化
	obj->Initialize(model_, pos, velocity);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);

	enemyIntervalShort_.push_back(obj);
}

void GameScene::AddEnemyStrong(Vector3 pos, Vector3 velocity) {
	// 生成
	EnemyStrong* obj = new EnemyStrong();
	// 初期化
	obj->Initialize(model_, pos, velocity);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);
	// レールカメラと親子関係を結ぶ
	obj->SetParent(&railCamera_->GetWorldTransform());

	enemyStrong_.push_back(obj);
}

void GameScene::AddEnemyMove(Vector3 pos, Vector3 velocity, const Vector3& leaveVelocity) {
	// 生成
	EnemyMove* obj = new EnemyMove();
	// 初期化
	obj->Initialize(model_, pos, velocity, leaveVelocity);
	// 自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// ゲームシーンを渡す
	obj->SetGameScene(this);

	enemyMove_.push_back(obj);
}

void GameScene::LoadEnemyPopData() {
	std::ifstream file;
	// 通常の敵
	// ファイルを開く
	file.open("Resources/script/enemyData.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopComands << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// 発射間隔が短い敵
	file.open("Resources/script/enemyIntervalShortData.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyIntervalShortPopComands << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// 強い敵
	// ファイルを開く
	file.open("Resources/script/enemyStrongData.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyStrongPopComands << file.rdbuf();

	// ファイルを閉じる
	file.close();

	// 動く敵
	// ファイルを開く
	file.open("Resources/script/enemyMoveData.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyMovePopComands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopComands() {
	// 待機処理
	if (isWait_ == true) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			// 待機完了
			isWait_ = false;
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

			// 敵を発生させる
			AddEnemy(Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait_ = true;
			waitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyIntervalShortPopComands() {
	// 待機処理
	if (isWait_ == true) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			// 待機完了
			isWait_ = false;
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

			// 敵を発生させる
			AddEnemyIntervalShort(Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait_ = true;
			waitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyStrongPopComands() {
	// 待機処理
	if (isWait_ == true) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			// 待機完了
			isWait_ = false;
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

			// 敵を発生させる
			AddEnemyStrong(Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait_ = true;
			waitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::UpdateEnemyMovePopComands() {
	// 待機処理
	if (isWait_ == true) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			// 待機完了
			isWait_ = false;
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

			// 敵を発生させる
			AddEnemyMove(Vector3(x, y, z), Vector3(velocityX, velocityY, velocityZ),
			    Vector3(leaveVelocityX, leaveVelocityY, leaveVelocityZ));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait_ = true;
			waitTimer_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}
