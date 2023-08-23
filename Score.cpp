#include "Score.h"

Score::~Score() {
	delete sprite_;
	for (int32_t i = 0; i < 5; i++) {
		delete spriteNumber_[i];
	}
	delete spriteHigh_;
	for (int32_t i = 0; i < 5; i++) {
		delete spriteHighNumber_[i];
	}
}

void Score::Initialize() {
	// SCORE
	// ワールド変換
	worldTransform_.Initialize();
	worldTransform_.translation_ = {25.0f, 20.0f, 0.0f};

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("sprite/score.png");
	// スプライト生成
	sprite_ = Sprite::Create(
	    textureHandle_, Vector2{worldTransform_.translation_.x, worldTransform_.translation_.y},
	    {1, 1, 1, 1}, {0.0f, 0.0f});

	// number
	// ワールド変換
	worldTransformNumber_.Initialize();
	worldTransformNumber_.translation_ = {20.0f, 70.0f, 0.0f};

	// テクスチャ読み込み
	textureHandleNumber_[0] = TextureManager::Load("number/0.png");
	textureHandleNumber_[1] = TextureManager::Load("number/1.png");
	textureHandleNumber_[2] = TextureManager::Load("number/2.png");
	textureHandleNumber_[3] = TextureManager::Load("number/3.png");
	textureHandleNumber_[4] = TextureManager::Load("number/4.png");
	textureHandleNumber_[5] = TextureManager::Load("number/5.png");
	textureHandleNumber_[6] = TextureManager::Load("number/6.png");
	textureHandleNumber_[7] = TextureManager::Load("number/7.png");
	textureHandleNumber_[8] = TextureManager::Load("number/8.png");
	textureHandleNumber_[9] = TextureManager::Load("number/9.png");
	// スプライト生成
	for (int32_t i = 0; i < 5; i++) {
		spriteNumber_[i] = Sprite::Create(
		    textureHandleNumber_[eachNumber_[i]],
		    Vector2{
		        worldTransformNumber_.translation_.x + (width_ * i),
		        worldTransformNumber_.translation_.y},
		    {1, 1, 1, 1}, {0.0f, 0.0f});
	}

	// スコア
	score_ = 0;
	subtractScore_ = 50;
	add_ = false;

	number_ = score_; 

	// HIGHSCORE
	// ワールド変換
	worldTransformHigh_.Initialize();
	worldTransformHigh_.translation_ = {480.0f, 450.0f, 0.0f};

	// テクスチャ読み込み
	textureHandleHigh_ = TextureManager::Load("sprite/highScore.png");
	// スプライト生成
	spriteHigh_ = Sprite::Create(
	    textureHandleHigh_,
	    Vector2{worldTransformHigh_.translation_.x, worldTransformHigh_.translation_.y},
	    {1, 1, 1, 1}, {0.0f, 0.0f});

	// highNumber
	// ワールド変換
	worldTransformHighNumber_.Initialize();
	worldTransformHighNumber_.translation_ = {550.0f, 500.0f, 0.0f};

	// テクスチャ読み込み
	textureHandleHighNumber_[0] = TextureManager::Load("number/high0.png");
	textureHandleHighNumber_[1] = TextureManager::Load("number/high1.png");
	textureHandleHighNumber_[2] = TextureManager::Load("number/high2.png");
	textureHandleHighNumber_[3] = TextureManager::Load("number/high3.png");
	textureHandleHighNumber_[4] = TextureManager::Load("number/high4.png");
	textureHandleHighNumber_[5] = TextureManager::Load("number/high5.png");
	textureHandleHighNumber_[6] = TextureManager::Load("number/high6.png");
	textureHandleHighNumber_[7] = TextureManager::Load("number/high7.png");
	textureHandleHighNumber_[8] = TextureManager::Load("number/high8.png");
	textureHandleHighNumber_[9] = TextureManager::Load("number/high9.png");
	// スプライト生成
	for (int32_t i = 0; i < 5; i++) {
		spriteHighNumber_[i] = Sprite::Create(
		    textureHandleHighNumber_[eachHighNumber_[i]],
		    Vector2{
		        worldTransformHighNumber_.translation_.x + (width_ * i),
		        worldTransformHighNumber_.translation_.y},
		    {1, 1, 1, 1}, {0.0f, 0.0f});
	}

	// スコア更新メッセージ
	// ワールド変換
	worldTransformScoreUpdate_.Initialize();
	worldTransformScoreUpdate_.translation_ = {670.0f, 415.0f, 0.0f};

	// テクスチャ読み込み
	textureHandleScoreUpdate_ = TextureManager::Load("sprite/scoreUpdate.png");
	// スプライト生成
	spriteScoreUpdate_ = Sprite::Create(
	    textureHandleScoreUpdate_,
	    Vector2{
	        worldTransformScoreUpdate_.translation_.x, worldTransformScoreUpdate_.translation_.y},
	    {1, 1, 1, 1}, {0.0f, 0.0f});

	isScoreUpdate = false;
}

void Score::Update() {
	// スコア
	// 画像割り当て
	eachNumber_[0] = number_ / 10000;
	number_ = number_ % 10000;

	eachNumber_[1] = number_ / 1000;
	number_ = number_ % 1000;

	eachNumber_[2] = number_ / 100;
	number_ = number_ % 100;

	eachNumber_[3] = number_ / 10;

	eachNumber_[4] = number_ % 10;

	number_ = score_;

	// スコア制限
	if (score_ <= 0) {
		score_ = 0;
		number_ = 0;
	}
	if (score_ > 99999) {
		score_ = 99999;
		number_ = 99999;
	}

	if (add_) {
		subtractScore_ += 50;
		add_ = false;
	}

	for (int32_t i = 0; i < 5; i++) {
		spriteNumber_[i]->SetTextureHandle(textureHandleNumber_[eachNumber_[i]]);
	}

	// ハイスコア
	// 画像割り当て
	eachHighNumber_[0] = highNumber_ / 10000;
	highNumber_ = highNumber_ % 10000;

	eachHighNumber_[1] = highNumber_ / 1000;
	highNumber_ = highNumber_ % 1000;

	eachHighNumber_[2] = highNumber_ / 100;
	highNumber_ = highNumber_ % 100;

	eachHighNumber_[3] = highNumber_ / 10;

	eachHighNumber_[4] = highNumber_ % 10;

	highNumber_ = highScore_;

	// スコアがハイスコアを下回ったらハイスコアを前回の記録に戻す
	if (highScore_ >= score_) {
		if (isScoreUpdate) {
			highScore_ = highScoreLast_;
			isScoreUpdate = false;
		}
	}

	// ハイスコア更新
	if (highScore_ < score_) {
		if (isScoreUpdate == false) {
			highScore_ = score_;
			isScoreUpdate = true;
		}
	}

	for (int32_t i = 0; i < 5; i++) {
		spriteHighNumber_[i]->SetTextureHandle(textureHandleHighNumber_[eachHighNumber_[i]]);
	}
}

void Score::DrawUI(bool drawHighScore) {
	// SCORE
	sprite_->Draw();

	// number
	for (int32_t i = 0; i < 5; i++) {
		spriteNumber_[i]->Draw();
	}

	if (drawHighScore) {
		// HIGHSCORE
		spriteHigh_->Draw();

		// highNumber
		for (int32_t i = 0; i < 5; i++) {
			spriteHighNumber_[i]->Draw();
		}

		// スコア更新メッセージ
		if (isScoreUpdate) {
			spriteScoreUpdate_->Draw();
		}
	}
}

void Score::AddScore50() { score_ += 50; }

void Score::AddScore100() { score_ += 100; }

void Score::AddScore300() { score_ += 300; }

void Score::AddScore() { score_++; }

void Score::SubtractScore() {
	score_ -= subtractScore_;
	add_ = true;
}
