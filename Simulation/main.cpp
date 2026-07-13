#include "DxLib.h"
#include"GameDefine.h"
#include"SceneManager.h"
#include"AsoDxLib/Mouse.h"
#include"AsoDxLib/Keyboard.h"
#include<time.h>

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	ChangeWindowMode(1); // windowモードで起動

	SetGraphMode(GameDefine::WIDTH, GameDefine::HEIGHT, GameDefine::DEPTH); // ウィンドウサイズを16：9に

	SetMainWindowText("Simulation"); // ウィンドウのタイトル表示を変更

	if (DxLib_Init() == -1)           // ＤＸライブラリ初期化処理
	{
		return -1;                        // エラーが起きたら直ちに終了
	}

	SRand((int)time(NULL));

	std::unique_ptr<SceneManager>p_SceneManager = std::make_unique<SceneManager>();
	p_SceneManager->Init();

	SetDrawScreen(DX_SCREEN_BACK); // 裏画面に描画
	ChangeFont("Malgun Gothic");

	while (ProcessMessage() == 0)
	{
		ClearDrawScreen(); // 画面をまっさらにする

		Mouse::Update();
		Keyboard::Update();

		p_SceneManager->Update();
		p_SceneManager->Draw();

		ScreenFlip(); // 裏画面を表面に

		if (CheckHitKey(KEY_INPUT_ESCAPE)) return -1;
	}


	WaitKey();                                // キー入力待ち

	DxLib_End();                               // ＤＸライブラリ使用の終了処理

	return 0;                           // ソフトの終了 
}