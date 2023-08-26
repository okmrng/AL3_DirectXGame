#include "Explanation.h"

Explanation::Explanation() {}

Explanation::~Explanation() { delete sprite_; }

void Explanation::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// 背景
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("sprite/explanation.png");
	// スプライト生成
	sprite_ = Sprite::Create(textureHandle_, {0, 0}, {1, 1, 1, 1}, {0.0f, 0.0f});

	// SPACE
	// テクスチャ読み込み
	textureHandleSpace_ = TextureManager::Load("sprite/space.png");
	// スプライト生成
	spriteSpace_ = Sprite::Create(textureHandleSpace_, {1134, 641}, {1, 1, 1, 1}, {0.0f, 0.0f});

	toNext_ = false;

	keyCount_ = 5;

	// BGM
	soundHandle_ = audio_->LoadWave("bgm/explanation.wav");
	voiceHandle_ = 0u;
}

void Explanation::Update() {
	// BGM再生
	if (!audio_->IsPlaying(voiceHandle_)) {
		voiceHandle_ = audio_->PlayWave(soundHandle_, true, 0.2f);
	}

	keyCount_--;
	if (keyCount_ <= 0) {
		if (input_->TriggerKey(DIK_SPACE)) {
			// BGM停止
			audio_->StopWave(voiceHandle_);

			// 次のシーンへ
			toNext_ = true;
		}
	}
}

void Explanation::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	sprite_->Draw();
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
	spriteSpace_->Draw();
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion 
}
