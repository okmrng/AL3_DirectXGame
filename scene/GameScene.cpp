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

	AddEnemy({0.0f, 5.0f, 30.0f}, {0.1f, 0.1f, -0.2f});

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

		// 敵
		UpdateEnemyPopComands();
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
	//判定対象BとAの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();

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

		// 敵の描画
		for (Enemy* enemy : enemy_) {
			enemy->Draw(viewProjection_);
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
	// 敵の生成
	Enemy* obj = new Enemy();
	// 敵の初期化
	obj->Initialize(model_, pos, velocity);
	// 敵キャラに自キャラのアドレスを渡す
	obj->SetPlayer(player_);
	// 敵キャラにゲームシーンを渡す
	obj->SetGameScene(this);

	enemy_.push_back(obj);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyData.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopComands << file.rdbuf();

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
