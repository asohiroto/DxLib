#include"DxLib.h"
#include"SceneMain.h"
#include"Pad.h"
#include"Player.h"
#include"Mouse.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1920, 1080, 32);

	if (DxLib_Init() == -1) // DXライブラリ初期化処理
	{
		return -1; // エラーが起きたら直ちに終了
	}

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	SceneMain* m_pScene = new SceneMain;
	m_pScene->Init();

	while (ProcessMessage() == 0)
	{
		LONGLONG time = GetNowHiPerformanceCount();

		// 画面のクリア
		ClearDrawScreen();

		// マウスカーソルの表示
		SetMouseDispFlag(TRUE);

		Pad::Update();
		Mouse::Update();

		m_pScene->Draw();
		m_pScene->Update();
		
		// 裏画面と表画面を入れ替える
		ScreenFlip();

		// fpsを６０に固定
		while (GetNowHiPerformanceCount() - time < 16667)
		{
		}

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
	}

	DxLib_End(); // DXライブラリ仕様の終了処理

	return 0; // ソフトの終了
}