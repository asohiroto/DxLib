#pragma once
#include "SceneBase.h"
#include "../General/StageIndex.h"
#include <memory>
class Input;
class Fader;
class SceneController;
class CameraController;
class SelectTitleUI;
class ActorManager;
class SE;
class EffekseerManager;
class DialogUI;
class TitleScene :
    public SceneBase
{
public:
    TitleScene(SceneController& controller);
    ~TitleScene();
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

    //メニュー
    enum class SelectMenuTitle : int
    {
        Continue = 0,
        NewGame = 1,
        Option = 2,
        GameEnd = 3
    };
private:
    //最初の激しいグリッジを行うフレームをカウント
    int m_hardShakingCountFrame;
    //各項目の値
    float m_blockScele;
    float m_noiseSpeed;
    float m_shakeStrength;

    //アクターマネージャー
    std::shared_ptr<ActorManager> m_actorManager;

    //エフェクト
    EffekseerManager& m_effectManager;

    //SE
    std::weak_ptr<SE> m_noiseSE;

    //UI
    std::weak_ptr<SelectTitleUI> m_selectTitleUI;
    std::weak_ptr<DialogUI> m_dialogUI;

    //セレクト番号
    SelectMenuTitle m_selectIndex;

    //状態に合わせた更新
    using UpdateFunc = void(TitleScene::*)(Input& input, Fader& fader);
    UpdateFunc m_update;
private:
    //タイトル更新
    void UpdateTitle(Input& input, Fader& fader);
    //モードセレクト更新
    void UpdateSelect(Input& input, Fader& fader);
    //ダイアログ更新
    void UpdateDialog(Input& input, Fader& fader);

private:
    //グリッジの更新
    void UpdateGlitch();
};

