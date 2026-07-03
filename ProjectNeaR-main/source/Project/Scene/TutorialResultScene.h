#pragma once
#include "SceneBase.h"
#include "../General/StageIndex.h"
#include <memory>
class Input;
class SceneController;
class ResultUI;
class Timer;
class TutorialResultUI;

class TutorialResultScene :
    public SceneBase
{
public:
    TutorialResultScene(SceneController& controller, StageIndex index);
    ~TutorialResultScene();
    /// <summary>
    /// 派生クラスで実装を実装
    /// virtual はこれを継承するかもしれないから
    /// overrideは明示的にオーバーライドをエディタに示すため
    /// </summary>
    virtual void Init()override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void End() override;
    virtual void Restart() override {};
    //デバッグ用
    virtual void DebugDraw() const override;
    //メニューインデックス
    enum class Menu : int
    {
        Next = 0,
        Select = 1
    };
private:
    //リザルトのUI
    std::weak_ptr<TutorialResultUI> m_mcUI;
    //チュートリアル名
    StageIndex m_tutorialIndex;
    //メニューインデックス
    Menu m_menuIndex;
};

