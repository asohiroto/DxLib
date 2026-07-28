#include "Scenes/SceneManager.h"
#include <DxLib.h>
#include <memory>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_ int nCmdShow)
{

	// 一部の関数はDxLib_Init()の前に実行する必要がある
	ChangeWindowMode(true);

	//画面サイズの変更
	//横1280,縦720、カラービット数32で設定
	SetGraphMode(1280, 720, 32);

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	// フルスクリーン切り替え時におかしくならないように
	SetChangeScreenModeGraphicsSystemResetFlag(false);

	// Zバッファの設定
	//Zバッファを使用する
	SetUseZBuffer3D(true);

	//Zバッファに書き込みを行う
	SetWriteZBuffer3D(true);

	//ポリゴンのバックカリングを使用する
	SetUseBackCulling(true);

	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);

	std::unique_ptr<SceneManager> p_SceneManager = std::make_unique<SceneManager>();
	p_SceneManager->Init();

	// フレームのカウンタ
	int _frameCount = 0;

	// ゲームループ
	while (ProcessMessage() != -1)
	{
		// このフレームの開始時刻を覚えておく
		LONGLONG start = GetNowHiPerformanceCount();
		// 1fに掛かった時間(ms)
		LONGLONG _oneFrameTime = GetNowHiPerformanceCount() - start;

		// 描画を行う前に画面をクリアする
		ClearDrawScreen();

		_frameCount++;

		p_SceneManager->Update();
		p_SceneManager->Draw();


		//表示
		DrawFormatString(0.0f, 0.0f, 0xffffff, "経過フレーム：%d", _frameCount);


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

		}
	}


	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}