#include "DxLib.h"
#include"SceneGame.h"
#include"GameDefine.h"
#include<memory>

using namespace GameDefine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	// 一部の関数はDxLib_Init()の前に実行する必要がある
	ChangeWindowMode(true);

	//画面サイズの変更
	//横1280,縦720、カラービット数32で設定
	SetGraphMode(WIDTH, HEIGHT, COLOR_BIT);

	// DirectX11をw使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	// フルスクリーン切り替え時におかしくならないように
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	// Zバッファの設定
	// Zバッファを使用する
	SetUseZBuffer3D(true);
	// Zバッファに書き込みを行う
	SetWriteZBuffer3D(true);
	// ポリゴンのバックカリングを使用する
	SetUseBackCulling(true);
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	std::unique_ptr<SceneGame> p_SceneGame = std::make_unique<SceneGame>();
	p_SceneGame->Init();

	 ChangeFont("Noto Serif JP Black");

	// ゲームループ
	while (ProcessMessage() != -1)
	{
		// このフレームの開始時刻を覚えておく
		LONGLONG start = GetNowHiPerformanceCount();

		// 描画を行う前に画面をクリアする
		ClearDrawScreen();

		p_SceneGame->Update();
		p_SceneGame->Draw();

		// 画面が切り替わるのを待つ
		ScreenFlip();

		// escキーでゲーム終了
		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// FPS60に固定する
		while (GetNowHiPerformanceCount() - start < 16667)
		{
			// 16.66ミリ秒(16667マイクロ秒)経過するまで待つ
			Sleep(0);
		}
	}

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}