#include "Score.h"

Score::~Score() {
	delete sprite_;
	for (int32_t i = 0; i < 5; i++) {
		delete spriteNumber_[i];
	}
}

void Score::Initialize() {
	audio_ = Audio::GetInstance();

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

	soundDamage_ = audio_->LoadWave("se/damage.wav");
	voiceDamage_ = 0u;

	// ランク
	// テクスチャ読み込み
	textureRank_ = TextureManager::Load("sprite/rank.png");
	// スプライト生成
	spriteRank_ = Sprite::Create(textureRank_, {420, 430}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// テクスチャ読み込み
	textureS_ = TextureManager::Load("sprite/s.png");
	// スプライト生成
	spriteS_ = Sprite::Create(textureS_, {470, 370}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// サウンド読み込み
	soundS_ = audio_->LoadWave("se/s.wav");
	voiceS_ = 0u;
	stopCountS_ = 120;

	// テクスチャ読み込み
	textureA_ = TextureManager::Load("sprite/a.png");
	// スプライト生成
	spriteA_ = Sprite::Create(textureA_, {470, 370}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// サウンド読み込み
	soundA_ = audio_->LoadWave("se/a.wav");
	voiceA_ = 0u;
	stopCountA_ = 60;

	// テクスチャ読み込み
	textureB_ = TextureManager::Load("sprite/b.png");
	// スプライト生成
	spriteB_ = Sprite::Create(textureB_, {520, 390}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// サウンド読み込み
	soundB_ = audio_->LoadWave("se/b.wav");
	voiceB_ = 0u;
	stopCountB_ = 60;

	// テクスチャ読み込み
	textureC_ = TextureManager::Load("sprite/c.png");
	// スプライト生成
	spriteC_ = Sprite::Create(textureC_, {520, 430}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// サウンド読み込み
	soundC_ = audio_->LoadWave("se/c.wav");
	voiceC_ = 0u;
	stopCountC_ = 60;
}

void Score::Update(bool playSound) {
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

	// ランクに応じてSEを再生
	if (playSound) {
		--stopCountS_;
		--stopCountA_;
		--stopCountB_;
		--stopCountC_;

		if (score_ >= 65000) {
			if (!audio_->IsPlaying(voiceS_)) {
				voiceS_ = audio_->PlayWave(soundS_);
			}
			if (stopCountS_ <= 0) {
				audio_->StopWave(voiceS_);
			}
		}
		if (score_ >= 60000 && score_ < 65000) {
			if (!audio_->IsPlaying(voiceA_)) {
				voiceA_ = audio_->PlayWave(soundA_);
			}
			if (stopCountA_ <= 0) {
				audio_->StopWave(voiceA_);
			}
		}
		if (score_ >= 55000 && score_ < 60000) {
			if (!audio_->IsPlaying(voiceB_)) {
				voiceB_ = audio_->PlayWave(soundB_);
			}
			if (stopCountB_ <= 0) {
				audio_->StopWave(voiceB_);
			}
		}
		if (score_ < 55000) {
			if (!audio_->IsPlaying(voiceC_)) {
				voiceC_ = audio_->PlayWave(soundC_,true);
			}
			if (stopCountC_<=0) {
				audio_->StopWave(voiceC_);
			}
		}
	}
}

void Score::DrawUI(bool drawRank) {
	// SCORE
	sprite_->Draw();

	// number
	for (int32_t i = 0; i < 5; i++) {
		spriteNumber_[i]->Draw();
	}

	if (drawRank) {
		if (score_ >= 65000) {
			spriteS_->Draw();
		}
		if (score_ >= 60000&&score_<65000) {
			spriteA_->Draw();
		}
		if (score_ >= 55000 && score_ < 60000) {
			spriteB_->Draw();
		}
		if (score_ < 55000) {
			spriteC_->Draw();
		}
		spriteRank_->Draw();
	}
}

void Score::AddScore50() { score_ += 50; }

void Score::AddScore100() { score_ += 100; }

void Score::AddScore300() { score_ += 300; }

void Score::AddScore() { score_++; }

void Score::SubtractScore() {
	score_ -= subtractScore_;
	add_ = true;
	if (!audio_->IsPlaying(voiceDamage_)) {
		voiceDamage_ = audio_->PlayWave(soundDamage_);
	}
}
