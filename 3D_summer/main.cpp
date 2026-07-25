#include "DxLib.h"
#include"SceneManager.h"
#include "Input.h"
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

	std::unique_ptr<SceneManager> p_SceneManager = std::make_unique<SceneManager>();
	p_SceneManager->Init();

	std::shared_ptr<Input> p_Input = std::make_shared<Input>();
	p_Input->Init(DX_INPUT_PAD1);

	std::shared_ptr<Input> p_InputSub = std::make_shared<Input>();
	p_InputSub->Init(DX_INPUT_PAD2);

	int _frameCount = 0;

	ChangeFont("Noto Sans JP Black");

	// ゲームループ
	while (ProcessMessage() != -1)
	{
		_frameCount++;
	
		// このフレームの開始時刻を覚えておく
		LONGLONG start = GetNowHiPerformanceCount();

		// 描画を行う前に画面をクリアする
		ClearDrawScreen();

		p_Input->Update();
		p_InputSub->Update();
		p_SceneManager->Update(p_Input, p_InputSub);
		p_SceneManager->Draw();

		DrawFormatString(0, 0, 0xff0000, "%d", _frameCount);

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