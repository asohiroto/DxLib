#include "DxLib.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(1);          // windowモードで起動
	SetGraphMode(1280, 720, 32);  // ウィンドウサイズを16：9に
	SetMainWindowText("DxLibTemplate"); // ウィンドウのタイトル表示を変更

	if (DxLib_Init() == -1)       // ＤＸライブラリ初期化処理
	{
		return -1;                // エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK);

	int fpsCount = 0;
	int currentFps = 0;
	int startTime = GetNowCount();
	int totalFrames = 0;

	while (ProcessMessage() == 0)
	{
		// 1フレームの開始時間を高精度で取得
		LONGLONG start = GetNowHiPerformanceCount();

		ClearDrawScreen();

		// 画面にFPSを表示
		DrawFormatString(0, 0, GetColor(255, 255, 255), "FPS: %d", currentFps);
		DrawFormatString(0, 20, GetColor(255, 255, 255), "Total Frames: %d", totalFrames);

		ScreenFlip();

		fpsCount++;
		totalFrames++;

		if (GetNowCount() - startTime >= 1000)
		{
			currentFps = fpsCount;
			fpsCount = 0;
			startTime += 1000;
		}

		// Escapeキーが押されたらループを抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;

		// 60FPS（約16.6ms）を維持するためのウエイト処理
		while (GetNowHiPerformanceCount() - start < 16667) Sleep(0);
	}

	DxLib_End();                   // ＤＸライブラリ使用の終了処理
	return 0;                      // ソフトの終了 
}