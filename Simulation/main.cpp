#include "DxLib.h"
#include"GameDefine.h"
#include"MainScene.h"
#include<time.h>

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

    ChangeWindowMode(1); // windowモードで起動

    SetGraphMode(GameDefine::WIDTH, GameDefine::HEIGHT, GameDefine::DEPTH); // ウィンドウサイズを16：9に

    SetMainWindowText("DxLibTemplate"); // ウィンドウのタイトル表示を変更

    if (DxLib_Init() == -1)           // ＤＸライブラリ初期化処理
    {
        return -1;                        // エラーが起きたら直ちに終了
    }

    SRand((int)time(NULL));

    MainScene* p_Main = new MainScene;

    p_Main->Init();

    SetDrawScreen(DX_SCREEN_BACK); // 裏画面に描画

    while (ProcessMessage() == 0)
    {
        ClearDrawScreen(); // 画面をまっさらにする

        p_Main->Update();
        p_Main->Draw();

        ScreenFlip(); // 裏画面を表面に

        if (CheckHitKey(KEY_INPUT_ESCAPE)) return -1;
    }

    delete p_Main;

    WaitKey();                                // キー入力待ち

    DxLib_End();                               // ＤＸライブラリ使用の終了処理

    return 0;                           // ソフトの終了 
}