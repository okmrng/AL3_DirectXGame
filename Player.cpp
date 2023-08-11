#include "Player.h"

#include <cassert>
#include "ImGuiManager.h"
#include "MathUtility.h"

Player::~Player() { 
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	
	delete sprite2DReticle_;
}

void Player::Initialize(Model* model) {
	//NULLポインタチェック
	assert(model);

	//引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// テクスチャの読み込み
	textureHandle_ = TextureManager::Load("player/player.png");

	//ワールド変換の初期化
	worldTransform_.Initialize();
	// 引数で受け取った情報をセット
	worldTransform_.translation_ = {0.0f, 0.0f, 15.0f};

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	model3DReticle_ = model;

	// レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("target.png");

	// スプライト生成
	sprite2DReticle_ = Sprite::Create(textureReticle, {640, 360}, {1, 1, 1, 1}, {0.5f, 0.5f});
}

void Player::Update(ViewProjection& viewProjection) {
	//行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//キャラクターの移動ベクトル
	Vector3 move{0, 0, 0};

	//キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	//押した方向でベクトル移動(左右)
	if (input_->PushKey(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}

	//押した方向でベクトル移動(上下)
	if (input_->PushKey(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	//範囲制限
	const float kMoveLimitX = 34.0f;
	const float kMoveLimitY = 18.0f;

	//範囲を越えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	worldTransform_.translation_.x += move.x;
	worldTransform_.translation_.y += move.y;
	worldTransform_.translation_.z += move.z;

	//行列更新
	worldTransform_.UpdateMatrix();

	//回転速さ(ラジアン/frame)
	const float kRotSpeed = 0.02f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
	if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += kRotSpeed;
	}

	//キャラクター攻撃処理
	Attack();

	//弾更新
	//if (bullet_!=nullptr)と同じ条件
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	//デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	//キャラクターの座標を表示
	ImGui::SetNextWindowPos({60, 60});
	ImGui::SetNextWindowSize({300, 100});

	ImGui::Begin("Player");
	float sliderValue[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};
	ImGui::SliderFloat3("position", sliderValue, -20.0f, 20.0f);
	worldTransform_.translation_ = {sliderValue[0], sliderValue[1], sliderValue[2]};
	ImGui::End();

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	{
		POINT mousePosition;

		// マウス座標(スクリーン座標)を取得する
		GetCursorPos(&mousePosition);

		// クライアントエリア座標に変換する
		HWND hwnd = WinApp::GetInstance()->GetHwnd();
		ScreenToClient(hwnd, &mousePosition);

		// マウス座標を2Dレティクルのスプライトに代入する
		sprite2DReticle_->SetPosition(Vector2((float)mousePosition.x, (float)mousePosition.y));

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
		worldTransform3DReticle_.translation_ = posNear + mouseDirection * kDistanceTestObject;

		// ワールド行列の更新
		worldTransform3DReticle_.UpdateMatrix();
		// ワールド行列の転送
		worldTransform3DReticle_.TransferMatrix();

		// デバッグ文字
		ImGui::Begin("PlayerReticle");
		ImGui::Text("2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x,sprite2DReticle_->GetPosition().y);
		ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
		ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
		ImGui::Text("3DReticle:(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x,
		    worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
		ImGui::End();
	}
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	model3DReticle_->Draw(worldTransform3DReticle_, viewProjection, textureHandle_);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI() { 
	sprite2DReticle_->Draw();
}

void Player::Attack() { 
	if (input_->PushKey(DIK_SPACE)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		// Vector3 velocity(0, 0, kBulletSpeed);
		Vector3 velocity;
		velocity.x = worldTransform3DReticle_.translation_.x - worldTransform_.matWorld_.m[3][0];
		velocity.y = worldTransform3DReticle_.translation_.y - worldTransform_.matWorld_.m[3][1];
		velocity.z = worldTransform3DReticle_.translation_.z - worldTransform_.matWorld_.m[3][2];

		// 正規化
		velocity = Normalize(velocity);

		velocity.x *= kBulletSpeed;
		velocity.y *= kBulletSpeed;
		velocity.z *= kBulletSpeed;

		// 速度ベクトルを自機の向きに合わせて回転させる
		//velocity = TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPositiopn(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::OnColision() {}

Vector3 Player::GetWorldPositiopn() {
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::SetParent(const WorldTransform* parent) {
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}
