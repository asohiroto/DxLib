#pragma once
#include "CameraBase.h"
#include <memory>
class ActorManager;
class BossCamera :
    public CameraBase
{
public:
    BossCamera(Vector3 bossPos, Vector3 bossDir, float distance, std::weak_ptr<ActorManager> pActorManager,bool isStart);
    ~BossCamera();
    void Init() override;
    void Update() override;
    void Draw() const override;

private:
    //ボスの座標
    Vector3 m_bossPos;
    Vector3 m_bossDir;
    //アクターマネージャーの参照
    std::weak_ptr<ActorManager> m_pActorManager;

    //フレームカウント
    int m_countFrame;

    //スタート状態
    bool m_isStart;
};