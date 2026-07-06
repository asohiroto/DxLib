#include "DxLib.h"
#include"Player.h"
#include"Camera.h"
#include"Object.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	ChangeWindowMode(1);          // windowモードで起動
	SetGraphMode(1280, 720, 32);  // ウィンドウサイズを16：9に
	SetUseDirect3DVersion(DX_DIRECT3D_11); // DirectXを使用
	SetMainWindowText("DxLibTemplate"); // ウィンドウのタイトル表示を変更

	if (DxLib_Init() == -1)       // ＤＸライブラリ初期化処理
	{
		return -1;                // エラーが起きたら直ちに終了
	}
	// フルスクリーンの切り替え時におかしくならないように
	SetChangeScreenModeGraphicsSystemResetFlag(false);
	// zバッファの設定
	// zバッファを使用する
	SetUseZBuffer3D(true);
	// zバッファに書き込みを行う
	SetWriteZBuffer3D(true);
	// ポリゴンのバックカリングを使用する
	SetUseBackCulling(true);
	// ダブルバッファモード
	SetDrawScreen(DX_SCREEN_BACK);
	// 各インスタンスを作成
	Player _player;
	Camera _camera;
	Object _object;
	// 各初期化
	_player.Init();
	_camera.Init();
	_object.Init();

	while (ProcessMessage() == 0)
	{
		// 1フレームの開始時間を高精度で取得
		LONGLONG start = GetNowHiPerformanceCount();

		ClearDrawScreen();

		_player.Update(_camera.GetCameraAngle());

		_camera.Update(_player.GetPos());

		// ゲーム画面の描画

		_object.Draw();

		_player.Draw();


		ScreenFlip();

		// Escapeキーが押されたらループを抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE)) break;

		// 60FPS（約16.6ms）を維持するためのウエイト処理
		while (GetNowHiPerformanceCount() - start < 16667) Sleep(0);
	}

	DxLib_End();                   // ＤＸライブラリ使用の終了処理
	return 0;                      // ソフトの終了 
}