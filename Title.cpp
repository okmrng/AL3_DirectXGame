#include "Title.h"

Title::~Title() { 
	delete sprite_title_bg;
}

void Title::Initialize() {
	// 背景
	// テクスチャ読み込み
	textureHandle_title_bg = TextureManager::Load("background/sprite/title.png");

	// スプライト生成
	sprite_title_bg = Sprite::Create(textureHandle_title_bg, {0.0f, 0.0f});

	// キー
	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
}

void Title::Update() {
	// スペースキーが押されたらメインゲームへ
	if (input_->PushKey(DIK_SPACE)) {
		toPlay_ = true;
	}
}

void Title::Draw() { sprite_title_bg->Draw(); }