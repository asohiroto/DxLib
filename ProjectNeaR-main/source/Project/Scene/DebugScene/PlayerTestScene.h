#pragma once
#include "../SceneBase.h" 
#include <memory>
class Input;
class CameraController;
class SceneController;
class ActorManager;
class AttackManager;

class PlayerTestScene :
    public SceneBase
{
public:
    PlayerTestScene(SceneController& controller);
    ~PlayerTestScene();
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
    //アクターマネージャー
    std::shared_ptr<ActorManager> m_actorManager;

    //攻撃
    std::shared_ptr<AttackManager> m_attackManager;

};

