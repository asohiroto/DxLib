#pragma once
#include "SceneBase.h"
#include "../General/StageIndex.h"
#include "../General/Math/MyMath.h"
#include <memory>
#include <map>
class SceneController;
class MenuUI;
class Input;
class PauseScene :
    public SceneBase
{
public:
    PauseScene(SceneController& controller);
    ~PauseScene();
    //派生クラスで実装を実装
    virtual void Init()override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void End() override;
    virtual void Restart() override {};
    virtual void DebugDraw()const override {};
private:
    //ゲームに戻る
    int m_backGameHandle;
    //最初から
    int m_restartGameHandle;
    //設定
    int m_optionHandle;
    //撤退
    int m_returnHandle;
    //背景
    int m_backHandle;
    //カーソル
    int m_cursorHandle;
    //カーソル座標
    Vector2 m_cursorPos;
    //カーソルアングル
    float m_cursorAngle;

    //メニュー
    enum class MenuIndex : int
    {
        //ゲームに戻る
        BackGame = 0,
        //最初から
        RestartGame = 1,
        //設定
        Option = 2,
        //撤退
        Return = 3
    };

    MenuIndex m_menuIndex;


};

