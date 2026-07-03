#pragma once
#include "../Actor.h"
#include "../../../General/Math/MyMath.h"
#include <DxLib.h>
#include <memory>
class Collidable;
class NormalEffect;
class ActorManager;
class StageObject :
    public Actor
{
public:
    StageObject(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager);
    ~StageObject();
    void Init()override;
    void Update() override;
    void OnCollide(const std::shared_ptr<Collidable> other)override;
    void Draw()const override;
    void DrawShadow()const override;
    void Complete() override;
    void End()override;
    //‚·‚è”²‚¯‚é‚©
    void SetIsThrough(bool isThrough) { m_isThrough = isThrough; };
    //•`‰æ‚·‚é‚©
    void SetIsDraw(bool isDraw) { m_isDraw = isDraw; };
    //‘å‚«‚³‚ð•Ô‚·
    float GetScaleZ()const;
    //ƒ‚ƒfƒ‹‚ÌŒü‚«
    Vector3 GetDir()const;
private:
    //•`‰æ‚·‚é‚©
    bool m_isDraw;
};


