#pragma once
#include "../UIBase.h"
#include "../../../General/Math/MyMath.h"
#include "../../../General/StageIndex.h"    
#include "../../../Scene/GameoverScene.h"    
#include <memory>
#include <string>
class Timer;
class GameoverUI :
    public UIBase
{
public:
    GameoverUI();
    ~GameoverUI();
    //更新
    void Update() override;
    //描画
    void Draw()const override;
    //インデックス
    void SetMenuIndex(GameoverScene::Menu index) { m_menuIndex = index; };
    //動きが終了したか
    bool IsFinish()const { return m_isFinish; };

private:
    //背景
    int m_backHandle;
	//ゲームオーバー文字
	int m_gameoverHandle;
    //続きから
	int m_continueHandle;
    //はじめから
	int m_restartHandle;
	//ステージセレクトへ
	int m_stageSelectHandle;
    //カーソル
    int m_cursorHandle;
private:
    //フレームカウント
	int m_countFrame;
    //ゲームオーバー文字の位置
	Vector2 m_gameoverPos;
    //メニューの位置
    Vector2 m_menuPos;
	//カーソルの位置
	Vector2 m_cousorPos;
	//メニュー
    GameoverScene::Menu m_menuIndex;
	//動きが終了したか
	bool m_isFinish;
    
};

