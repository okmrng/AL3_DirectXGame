#include "Title.h"
#include "TextureManager.h"
#include <cassert>

Title::Title() {}

Title::~Title() { 
	delete sprite_title_bg;
}

void Title::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	// 背景
	// テクスチャ読み込み
	textureHandle_title_bg = TextureManager::Load("sprite/title.png");

	// スプライト生成
	sprite_title_bg = Sprite::Create(textureHandle_title_bg, {0.0f, 0.0f});

	toNext_ = false;

	// BGM
	soundDataHandle_ = audio_->LoadWave("bgm/title.wav");
}

void Title::Update() {
	// BGM再生
	if (!audio_->IsPlaying(soundDataHandle_)) {
		audio_->PlayWave(soundDataHandle_, true, 0.2f);
	}

	// スペースキーが押されたらメインゲームへ
	if (input_->TriggerKey(DIK_SPACE)) {
		audio_->StopWave(soundDataHandle_);
		toNext_ = true;
	}
}

void Title::Draw() { 
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	sprite_title_bg->Draw();
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