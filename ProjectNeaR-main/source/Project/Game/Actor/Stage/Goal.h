#pragma once
#include "../Actor.h"
#include "../../../General/Math/MyMath.h"
#include <DxLib.h>
#include <memory>
class Collidable;
class NormalEffect;
class ActorManager;
class Goal :
    public Actor
{
public:
    Goal(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager);
    ~Goal();
    void Init()override;
    void Update() override;
    void OnCollide(const std::shared_ptr<Collidable> other)override;
    void Draw()const override;
    void DrawShadow()const override {};
    void Complete() override;
    void End()override;
    
private:
    std::weak_ptr<NormalEffect> m_effect;
 
};


