#pragma once
#include "../Character/CharacterBase.h"
#include "../../../General/Math/MyMath.h"
#include "../../../General/Collision/Rigidbody.h"
class CharaStatusData;
class ActorData;
class ActorManager;
class DebugEnemy :
	public CharacterBase
{
public:
	DebugEnemy(std::shared_ptr<ActorData> actorData, std::shared_ptr<CharaStatusData> charaStatusData,std::weak_ptr<ActorManager> pActorManager);
	~DebugEnemy();
	virtual void Init() override;
	virtual void Update() override;
	virtual void Draw() const override;
	virtual void End() override;
	void OnCollide(const std::shared_ptr<Collidable> other) override;
	void Complete();
	void SetPos(const Vector3& pos) { m_rb->m_pos = pos; }
private:
};

