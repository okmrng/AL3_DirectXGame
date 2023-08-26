#include "Audio.h"
#include "AxisIndicator.h"
#include "DirectXCommon.h"
#include "Title.h"
#include "Explanation.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include "WinApp.h"

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	WinApp* win = nullptr;
	DirectXCommon* dxCommon = nullptr;
	// 汎用機能
	Input* input = nullptr;
	Audio* audio = nullptr;
	AxisIndicator* axisIndicator = nullptr;
	PrimitiveDrawer* primitiveDrawer = nullptr;
	Title* title = nullptr;
	Explanation* explanation = nullptr;
	GameScene* gameScene = nullptr;

	// シーン
	enum class Scene {
		TITLE,       // タイトル
		EXPLANATION, // ゲーム説明
		MAINGAME,    // メインゲーム
		INITIALIZE   // 初期化
	};
	Scene scene = Scene::TITLE; // シーン

	// ゲームウィンドウの作成
	win = WinApp::GetInstance();
	win->CreateGameWindow(
	    L"SCOREGETTER", WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME), 1280, 720);
	win->SetSizeChangeMode(WinApp::SizeChangeMode::kNone);

	// DirectX初期化処理
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

#pragma region 汎用機能初期化
	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// 入力の初期化
	input = Input::GetInstance();
	input->Initialize();

	// オーディオの初期化
	audio = Audio::GetInstance();
	audio->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon->GetDevice());
	TextureManager::Load("white1x1.png");

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3Dモデル静的初期化
	Model::StaticInitialize();

	// 軸方向表示初期化
	axisIndicator = AxisIndicator::GetInstance();
	axisIndicator->Initialize();

	primitiveDrawer = PrimitiveDrawer::GetInstance();
	primitiveDrawer->Initialize();
#pragma endregion
	// タイトルの初期化
	title = new Title();
	title->Initialize();

	// ゲーム説明の初期化
	explanation = new Explanation();
	explanation->Initialize();

	// ゲームシーンの初期化
	gameScene = new GameScene();
	gameScene->Initialize();

	// メインループ
	while (true) {
		// メッセージ処理
		if (win->ProcessMessage()) {
			break;
		}

		// ImGui受付開始
		imguiManager->Begin();
		// 入力関連の毎フレーム処理
		input->Update();
		// タイトルの毎フレーム処理
		if (scene == Scene::TITLE) {
			title->Update();
			// タイトルからの遷移
			// ゲーム説明へ
			if (title->GetToNext()) {
				scene = Scene::EXPLANATION;
			}
		}
		// ゲーム説明の毎フレーム処理
		if (scene == Scene::EXPLANATION) {
			explanation->Update();
			// ゲーム説明からの遷移
			// メインゲームへ
			if (explanation->GetToNext()) {
				scene = Scene::MAINGAME;
			}
		}
		// ゲームシーンの毎フレーム処理
		if (scene == Scene::MAINGAME) {
			gameScene->Update();
			if (gameScene->GetToTitle()) {
				scene = Scene::INITIALIZE;
			}
		}
		// 初期化
		if (scene == Scene::INITIALIZE) {
			title->Initialize();
			explanation->Initialize();
			gameScene->Initialize();
			scene = Scene::TITLE;
		}
		// 軸表示の更新
		axisIndicator->Update();
		// ImGui受付終了
		imguiManager->End();

		// 描画開始
		dxCommon->PreDraw();
		// タイトルの描画
		if (scene == Scene::TITLE) {
			title->Draw();
		}
		// ゲーム説明の描画
		if (scene == Scene::EXPLANATION) {
			explanation->Draw();
		}
		// ゲームシーンの描画
		if (scene == Scene::MAINGAME) {
			gameScene->Draw();
		}
		// 軸表示の描画
		axisIndicator->Draw();
		// プリミティブ描画のリセット
		primitiveDrawer->Reset();
		// ImGui描画
		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// 各種解放
	SafeDelete(title);
	SafeDelete(explanation);
	SafeDelete(gameScene);
	audio->Finalize();
	// ImGui解放
	imguiManager->Finalize();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	return 0;
}