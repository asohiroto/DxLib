#include"DxLib.h"
#include"SceneMain.h"
#include"Pad.h"
#include"Player.h"
#include"Mouse.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetGraphMode(1920, 1080, 32);

	// VSync待ちをしない（fpsを固定するため）
	SetWaitVSyncFlag(FALSE);

	if (DxLib_Init() == -1) // DXライブラリ初期化処理
	{
		return -1; // エラーが起きたら直ちに終了
	}

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	SceneMain* m_pScene = new SceneMain;
	m_pScene->Init();

	// 高精度カウンタの現在の値を取得
	LONGLONG frameStartTime = GetNowHiPerformanceCount();

	// ループした回数を数えるカウンター
	int fpsCount = 0;
	// 確定した現在のFPS値
	int currentFps = 0;
	// 1秒を測るための開始時間
	int startTime = GetNowCount();
	// 総フレーム数
	int totalFrames = 0;

	while (ProcessMessage() == 0)
	{

		// 画面のクリア
		ClearDrawScreen();

		// マウスカーソルの表示
		SetMouseDispFlag(TRUE);

		Pad::Update();
		Mouse::Update();

		m_pScene->Draw();
		m_pScene->Update();

		fpsCount++; // ループが回るたびにカウントを1増やす
		totalFrames++; // 総フレーム数も1増やす

		// 現在の時間から、前回の計測から1秒（1000ms）経ったかチェック
		if (GetNowCount() - startTime >= 1000)
		{
			currentFps = fpsCount; // 1秒間に入ったカウント数がそのままFPSになる
			fpsCount = 0;          // カウンターをリセット
			startTime = GetNowCount(); // 次の1秒のために時間をリセット
		}

		// 画面の左上（座標 20, 20）に白色でFPSを表示
		DrawFormatString(20, 20, GetColor(255, 255, 255), "FPS: %d", currentFps);
		DrawFormatString(20, 40, GetColor(255, 255, 255), "Total Frames: %d", totalFrames);

		// 裏画面と表画面を入れ替える
		ScreenFlip();

		// fpsを６０に固定
		while (GetNowHiPerformanceCount() - frameStartTime < 16667)
		{
			// 1ms待機
			Sleep(1);
		}

		// 高精度カウンタの現在の値を再度取得
		frameStartTime = GetNowHiPerformanceCount();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}
	}

	delete m_pScene;
	DxLib_End(); // DXライブラリ仕様の終了処理

	return 0; // ソフトの終了
}