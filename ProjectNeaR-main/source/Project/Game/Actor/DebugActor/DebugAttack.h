#pragma once
#include "../Actor.h"
#include "../../../General/Math/MyMath.h"
#include "../../../General/Collision/Rigidbody.h"
class DebugAttack :
	public Actor
{
public:
	DebugAttack(std::shared_ptr<ActorData> actorData, std::weak_ptr<ActorManager> pActorManager);
	~DebugAttack();
	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() const override;
	virtual void End() override;
	void OnCollide(const std::shared_ptr<Collidable> other) override;
	void Complete();
	void SetPos(const Vector3& pos) { m_rb->m_pos = pos; }
private:
};

