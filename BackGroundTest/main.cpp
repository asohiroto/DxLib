#include "DxLib.h"

// プログラムは WinMain から始まります
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{

	ChangeWindowMode(1); // windowモードで起動

	SetGraphMode(1280, 720, 32); // ウィンドウサイズを16：9に

	SetMainWindowText("DxLibTemplate"); // ウィンドウのタイトル表示を変更

	if (DxLib_Init() == -1)           // ＤＸライブラリ初期化処理
	{
		return -1;                        // エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// ゲームが始まる前に使用するフォントのデータを作成
	int handle = CreateFontToHandle("Chiller", 64, -1, -1);

	while (ProcessMessage() == 0)
	{
		ClearDrawScreen();

		// これは使用禁止
		/*ChangeFont("HGP行書体");
		SetFontSize(100);*/

		DrawStringToHandle(100, 100, "Font Change", GetColor(0, 255, 255), handle);
		DrawFormatStringToHandle(100, 200, GetColor(255, 255, 255), handle,"%1f", GetFPS());
		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE)) return -1;
	}

	WaitKey();                                // キー入力待ち

	DeleteFontToHandle(handle);

	DxLib_End();                               // ＤＸライブラリ使用の終了処理

	return 0;                           // ソフトの終了 
}