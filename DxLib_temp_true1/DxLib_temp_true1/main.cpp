#include "DxLib.h"
#include"GameDefine.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// windowモードで起動
	ChangeWindowMode(1);
	// ウィンドウのサイズ設定
	SetGraphMode(GameDefine::WIDTH, GameDefine::HEIGHT, GameDefine::DEPTH);

	SetMainWindowText("GameName");

	if (DxLib_Init() == -1)           // ＤＸライブラリ初期化処理
	{
		return -1;                        // エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK); // 裏画面に描画

	while (ProcessMessage() == 0)
	{
		ClearDrawScreen(); // 画面をまっさらにする

		// ここに処理

		ScreenFlip(); // 裏画面を表面に

		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;
	}


	WaitKey();                                // キー入力待ち

	DxLib_End();                               // ＤＸライブラリ使用の終了処理

	return 0;                           // ソフトの終了 
}
