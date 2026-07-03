#pragma once
#include "SceneBase.h"
#include "../General/StageIndex.h"
#include <memory>
class Input;
class SceneController;
class ResultUI;
class Timer;

class ResultScene :
    public SceneBase
{
public:
    ResultScene(std::wstring stageName,SceneController& controller, std::shared_ptr<Timer> timer);
    ~ResultScene();
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
private:
    //リザルトのUI
    std::weak_ptr<ResultUI> m_resultUI;
    //タイマー
    std::shared_ptr<Timer> m_timer;
    //ステージ名
    std::wstring m_stageName;
};

