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
	delete enemy_;

	// 天球の解放
	delete skydome_;

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
	player_->Initialize(model_, textureHandle_);

	// 敵の生成
	enemy_ = new Enemy();
	// 敵の初期化
	enemy_->Initialize(model_, {0, 0, 50});

	//敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_);

	// デバッグカメラ生成
	debugCamera_ = new DebugCamera(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向が参照するビュープロジェクションを参照する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	//自キャラの更新
	player_->Update();

	//敵の更新
	if (enemy_) {
		enemy_->Update();
	}

	// 当たり判定
	CheckAllCollisions();

	//天球の更新
	skydome_->Update();

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
	} else {
		//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
}

void GameScene::CheckAllCollisions() { 
	//判定対象BとAの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	//自キャラと敵弾の当たり判定
	#pragma region
	//自キャラの座標
	posA = player_->GetWorldPositiopn();

	//自キャラと敵弾全ての当たり判定
	for (EnemyBullet* bullet : enemyBullets) {
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
	// 敵キャラの座標
	posA = enemy_->GetWorldPositiopn();

	// 自弾と敵キャラ全ての当たり判定
	for (PlayerBullet* bullet : playerBullets) {
		// 自弾の座標
		posB = bullet->GetWorldPositiopn();

		// AとBの距離を求める
		const float collisionX = (posB.x - posA.x) * (posB.x - posA.x);
		const float collisionY = (posB.y - posA.y) * (posB.y - posA.y);
		const float collisionZ = (posB.z - posA.z) * (posB.z - posA.z);
		const float collisionXYZ = collisionX + collisionY + collisionZ;

		const float collisionRadiusA = enemy_->GetRadius();
		const float collisionRadiusB = bullet->GetRadius();
		const float collisionRadiusAB =
		    (collisionRadiusA + collisionRadiusB) * (collisionRadiusA + collisionRadiusB);

		// 球と球の交差判定
		if (collisionXYZ <= collisionRadiusAB) {
			// 自キャラの衝突時コールバックを呼び出す
			enemy_->OnColision();
			// 自弾の衝突時コールバックを呼び出す
			bullet->OnColision();
		}
	}
	#pragma endregion

	// 自弾と敵弾の当たり判定
	#pragma region

	// 自弾と敵キャラ全ての当たり判定
	for (PlayerBullet* pBullet : playerBullets) {
		for (EnemyBullet* eBullet : enemyBullets) {
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
				enemy_->OnColision();
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
	enemy_->Draw(viewProjection_);

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
