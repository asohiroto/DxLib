#pragma once
#include "../Actor.h"
#include "../../../General/Math/MyMath.h"
#include <DxLib.h>
class ActorManager;
class GameCamera;
class AttackManager;
class Model;
class Sky :
    public Actor
{
public:
	Sky(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager);
	~Sky();
    void Init()override {};
    void Update() override;
    void OnCollide(const std::shared_ptr<Collidable> other)override {};
    void Draw()const override;
    void DrawShadow()const override {};
    void Complete() override {};
    void End()override {};
private:
	
};

