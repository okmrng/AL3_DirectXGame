#include "Player.h"

#include "RailCamera.h"

Player::~Player() { 
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	delete bomb_;
	delete spriteReticle_;
}

void Player::Initialize(Model* model) {
	// 自機
	// NULLポインタチェック
	assert(model);
	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("player/player.png");

	// ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った情報をセット
	worldTransform_.translation_ = {0.0f, 0.0f, 15.0f};

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// レティクル
	// レティクルのワールドトランスフォーム初期化
	worldTransformReticle_.Initialize();

	model3DReticle_ = model;

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("player/target.png");
	// スプライト生成
	spriteReticle_ = Sprite::Create(textureReticle, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});
}

void Player::Update(ViewProjection& viewProjection) {
	// キャラクターの移動ベクトル
	Vector3 move{0, 0, 0};
	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向でベクトル移動
	if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_W)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_S)) {
		move.y -= kCharacterSpeed;
	}

	// 範囲制限
	const float kMoveLimitX = 9.5f;
	const float kMoveLimitY = 5.0f;

	// 範囲を越えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	// 行列更新
	worldTransform_.UpdateMatrix();
	// 行列の転送
	worldTransform_.TransferMatrix();

	// キャラクター攻撃処理
	if (--kAttackTimer_ <= 0) {
		Attack();
		kAttackTimer_ = kAttackInterval_;
	}

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->GetisDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// ボム
	Bomb();

	if (bomb_) {
		bomb_->Update();
	}

	// デスフラグが立ったらボムを削除
	if (bomb_) {
		if (bomb_->GetisDead()) {
			delete bomb_;
			bomb_ = nullptr;
		}
	}

	// ボムクールタイム
	if (canBomb_ == false) {
		if (--bombTimer_ <= 0) {
			canBomb_ = true;
		}
	}

	// マウスカーソルのスクリーン座標からワールド座標を取得して2Dレティクル配置
	UpdateReticle(viewProjection);

	// ゲームオーバー
	if (HP_ <= 0) {
		isDead_ = true;
	}
}

void Player::UpdateReticle(ViewProjection& viewProjection) {
	POINT mousePosition;

	// マウス座標(スクリーン座標)を取得する
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2Dレティクルのスプライトに代入する
	spriteReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

	// ビューポート行列
	Matrix4x4 matViewport =
	    MakeViewportMatrix(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight, 0.0f, 1.0f);

	// ビュープロジェクションビューポート合成行列
	Matrix4x4 matViewProjectionViewport =
	    viewProjection.matView * viewProjection.matProjection * matViewport;

	// 合成行列の逆行列
	Matrix4x4 matInverseViewProjectionViewport = Inverse(matViewProjectionViewport);

	// スクリーン座標
	Vector3 posNear = Vector3((float)mousePosition.x, (float)mousePosition.y, 0);
	Vector3 posFar = Vector3((float)mousePosition.x, (float)mousePosition.y, 1);

	// スクリーン座標系からワールド座標系へ
	posNear = Transform(posNear, matInverseViewProjectionViewport);
	posFar = Transform(posFar, matInverseViewProjectionViewport);

	// マウスレイの方向
	Vector3 mouseDirection = posFar - posNear;

	mouseDirection = Normalize(mouseDirection);

	// カメラから標準オブジェクトの距離
	const float kDistanceTestObject = 50.0f;
	worldTransformReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;

	// ワールド行列の更新
	worldTransformReticle_.UpdateMatrix();
	// ワールド行列の転送
	worldTransformReticle_.TransferMatrix();
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデル
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}

	model3DReticle_->Draw(worldTransformReticle_, viewProjection, textureHandle_);

	// ボム
	if (bomb_) {
		bomb_->Draw(viewProjection);
	}
}

void Player::DrawUI() { 
	spriteReticle_->Draw();
}

void Player::Attack() { 
	if (input_->IsPressMouse(0)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity;
		/*velocity.x = worldTransformReticle_.translation_.x - worldTransform_.matWorld_.m[3][0];
		velocity.y = worldTransformReticle_.translation_.y - worldTransform_.matWorld_.m[3][1];
		velocity.z = worldTransformReticle_.translation_.z - worldTransform_.matWorld_.m[3][2];*/

		Vector3 railCameraWorld = railCamera_->GetWorldPositiopn() + worldTransform_.translation_;
		
		velocity = worldTransformReticle_.translation_ - railCameraWorld;

		// 正規化
		velocity = Normalize(velocity);

		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		//newBullet->Initialize(model_, GetWorldPositiopn(), velocity);
		newBullet->Initialize(model_, railCameraWorld, velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::Bomb() {
	if (canBomb_) {
		if (input_->IsTriggerMouse(1)) {
			// 弾の速度
			const float kBulletSpeed = 1.0f;
			Vector3 velocity;
			/*velocity.x = worldTransformReticle_.translation_.x - worldTransform_.matWorld_.m[3][0];
			velocity.y = worldTransformReticle_.translation_.y - worldTransform_.matWorld_.m[3][1];
			velocity.z = worldTransformReticle_.translation_.z - worldTransform_.matWorld_.m[3][2];*/

			velocity = worldTransformReticle_.translation_ - railCamera_->GetWorldPositiopn();

			// 正規化
			velocity = Normalize(velocity);

			velocity.x *= kBulletSpeed;
			velocity.y *= kBulletSpeed;
			velocity.z *= kBulletSpeed;

			// 弾を生成し、初期化
			PlayerBomb* newBomb = new PlayerBomb();
			//newBomb->Initialize(model_, GetWorldPositiopn(), velocity);
			newBomb->Initialize(model_, railCamera_->GetWorldPositiopn(), velocity);

			// 弾を登録する
			bomb_ = newBomb;

			bombTimer_ = kCoolTime_;
			canBomb_ = false;
		}
	}
}

void Player::OnColision() { --HP_; }

Vector3 Player::GetWorldPositiopn() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	
	return worldPos;
}

Vector3 Player::GetBombWorldPositiopn() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	if (bomb_) {
		worldPos = bomb_->GetWorldPositiopn();
	}

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
	//worldTransformReticle_.parent_ = parent;
	/*for (PlayerBullet* bullet : bullets_) {
		bullet->SetParent(parent);
	}*/
}
