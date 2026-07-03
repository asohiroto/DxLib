#include "TrackActorEffect.h"
#include "../../Game/Actor/Actor.h"

TrackActorEffect::TrackActorEffect(int playHandle, std::weak_ptr<Actor> target):
	NormalEffect(playHandle, target.lock()->GetPos()),
	m_target(target)
{
}

TrackActorEffect::~TrackActorEffect()
{
}

void TrackActorEffect::Update()
{
	//ターゲットがいないなら削除
	if (m_target.expired())
	{
		Delete();
		return;
	}

	//ターゲット
	auto target = m_target.lock();

	//移動
	SetPos(target->GetPos());

	//タイムスケールを使う
	SetTimeScale(target->GetTimeScale());

	//更新
	NormalEffect::Update();
}
