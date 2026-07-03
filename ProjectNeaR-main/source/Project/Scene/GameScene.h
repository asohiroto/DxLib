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
class Timer;
class TutorialManager;

class GameScene :
    public SceneBase
{
public:
    GameScene(SceneController& controller,std::wstring stageName);
    ~GameScene();
    /// <summary>
    /// 派生クラスで実装を実装
    /// virtual はこれを継承するかもしれないから
    /// overrideは明示的にオーバーライドをエディタに示すため
    /// </summary>
    virtual void Init()override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void End() override;
    virtual void Restart() override;
    //デバッグ用
    virtual void DebugDraw() const override;
  
private:
    //アクターマネージャー
    std::shared_ptr<ActorManager> m_actorManager;

    //攻撃
    std::shared_ptr<AttackManager> m_attackManager;

    //エリアマネージャー
    std::shared_ptr<BattleAreaManager> m_battleAreaManager;

    //ステージ名
    std::wstring m_stageName;

    //エフェクト
    EffekseerManager& m_effectManager;

    //タイマー
    std::shared_ptr<Timer> m_timer;

    //ゲームオーバー
    bool m_isGameover;
    //ゲームクリア
    bool m_isGameClear;

private:
    //ボス戦に入ったらBGM変更
    void ChangeBossBattleBGM(bool isBeforeBossBattle);
    //ステージBGM
    void PlayStageBGM();
    //ステージBGM初期化
    void InitStageBGM();
};

