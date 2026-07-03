#pragma once
#include "SceneBase.h"
#include "../General/StageIndex.h"
#include <memory>
class SceneController;
class GameoverUI;
class GameoverScene :
    public SceneBase
{
public:
    GameoverScene(SceneController& controller);
    ~GameoverScene();
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
        Continue = 0,
        Restart = 1,
        StageSelect = 2,
    };
private:
    //UI
	std::weak_ptr<GameoverUI> m_pGameoverUI;
    //メニュー
    Menu m_menuIndex;
};

