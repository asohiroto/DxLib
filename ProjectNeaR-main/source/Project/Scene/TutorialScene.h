#pragma once
#include "SceneBase.h"
#include <memory>
#include <string>
class Input;
class CameraController;
class SceneController;
class ActorManager;
class BattleAreaManager;
class AttackManager;
class EffekseerManager;
class TutorialManager;

class TutorialScene :
    public SceneBase
{
public:
    TutorialScene(SceneController& controller, std::wstring stageName);
    ~TutorialScene();
    /// <summary>
    /// 派生クラスで実装を実装
    /// virtual はこれを継承するかもしれないから
    /// overrideは明示的にオーバーライドをエディタに示すため
    /// </summary>
    virtual void Init()override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void End() override;
    virtual void Restart() override ;
    //デバッグ用
    virtual void DebugDraw() const override;

private:
    //アクターマネージャー
    std::shared_ptr<ActorManager> m_actorManager;

    //攻撃
    std::shared_ptr<AttackManager> m_attackManager;

    //エリアマネージャー
    std::shared_ptr<BattleAreaManager> m_battleAreaManager;

    //カメラ
    std::shared_ptr<CameraController> m_cameraController;

    //ステージ名
    std::wstring m_stageName;

    //エフェクト
    EffekseerManager& m_effectManager;

    //チュートリアルマネージャー
    std::shared_ptr<TutorialManager> m_tutorialManager;
};

