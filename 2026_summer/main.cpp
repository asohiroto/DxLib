#include "Scenes/SceneManager.h"
#include "GameDefine.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
#include <memory>

using namespace GameDefine;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPSTR lpCmdLine, _In_ int nCmdShow)
{

	// 一部の関数はDxLib_Init()の前に実行する必要がある
	ChangeWindowMode(true);

	//画面サイズの変更
	//横1280,縦720、カラービット数32で設定
	SetGraphMode(WIDTH, HEIGHT, COLOR_BIT);

	// フォントを変更する
	ChangeFont("Noto Sans JP Black");

	// DirectX11を使用するようにする。(DirectX9も可、一部機能不可)
	// Effekseerを使用するには必ず設定する。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	// 引数には画面に表示する最大パーティクル数を設定する。
	if (Effkseer_Init(EffectParticleLimit) == -1)
	{
		DxLib_End();
	}

	// DXライブラリのデバイスロストした時のコールバックを設定する。
	// ウインドウとフルスクリーンの切り替えが発生する場合は必ず実行する。
	Effekseer_SetGraphicsDeviceLostCallbackFunctions();

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

#ifdef _DEBUG
		//表示
		DrawFormatString(0.0f, 0.0f, 0xffffff, "経過フレーム：%d", _frameCount);
#endif

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

	p_SceneManager.reset();

	Effkseer_End();
	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}