#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include "AxisIndicator.h"

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete modelSkydome_;

	//自キャラの解放
	delete player_;

	// 敵の解放
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	// 敵弾の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}

	// 天球の解放
	delete skydome_;

	// レールカメラの解放
	delete railCamera_;

	//デバックカメラの解放
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("sample.png");

	// 3Dモデルの生成
	model_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	Vector3 playerPosition(0.0f, 0.0f, 15.0f);
	player_->Initialize(model_, textureHandle_, playerPosition);

	AddEnemy({0.0f, 5.0f, 30.0f});

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_);

	// レールカメラの生成
	railCamera_ = new RailCamera();
	// レールカメラの初期化
	railCamera_->Initialize({0.0f,0.0f,1.0f,},{0.0f,0.0f,0.0f});
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向が参照するビュープロジェクションを参照する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	// 敵の更新
	for (Enemy* enemy : enemy_) {
		enemy->Update();
	}
	// デスフラグの立った敵を削除
	enemy_.remove_if([](Enemy* enemy) {
		if (enemy->GetIsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 当たり判定
	CheckAllCollisions();

	//天球の更新
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

		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} 
	else if (!isDebugCameraActive_) {
		viewProjection_.matView = railCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();
	}
}

void GameScene::CheckAllCollisions() { 
	//判定対象BとAの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	//const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	//自キャラと敵弾の当たり判定
	#pragma region
	//自キャラの座標
	posA = player_->GetWorldPositiopn();

	//自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : bullets_) {
		//敵弾の座標
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

	// 自弾と敵キャラの当たり判定
	#pragma region
	for (Enemy* enemy : enemy_) {
		// 敵キャラの座標
		posA = enemy->GetWorldPositiopn();

		// 自弾と敵キャラ全ての当たり判定
		for (PlayerBullet* bullet : playerBullets) {
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
				// 自キャラの衝突時コールバックを呼び出す
				enemy->OnColision();
				// 自弾の衝突時コールバックを呼び出す
				bullet->OnColision();
			}
		}
	}
	#pragma endregion

	// 自弾と敵弾の当たり判定
	#pragma region

	// 自弾と敵キャラ全ての当たり判定
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
	/// </summary>

	//自キャラの描画
	player_->Draw(viewProjection_);

	//敵の描画
	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}
	// 敵の弾の描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}

	// 天球の描画
	skydome_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	bullets_.push_back(enemyBullet);
}

void GameScene::AddEnemy(Vector3 pos) {
	// 敵の生成
	Enemy* obj = new Enemy();
	// 敵の初期化
	obj->Initialize(model_, pos);
	// 敵キャラに自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// 敵キャラのゲームシーンを渡す
	obj->SetGameScene(this);

	enemy_.push_back(obj);
}
