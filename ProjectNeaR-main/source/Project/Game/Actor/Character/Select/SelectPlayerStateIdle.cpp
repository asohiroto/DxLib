#include "SelectPlayerStateIdle.h"
#include "SelectPlayer.h"
#include "../../../../General/Model.h"

namespace
{
	//アニメーション
	const std::wstring kIdle = L"SelectIdle";
}

SelectPlayerStateIdle::SelectPlayerStateIdle(std::weak_ptr<Actor> player):
	CharacterStateBase(player,false)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<SelectPlayer>(m_pOwner.lock());

	//アニメーション
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
	owner->SetCollState(CollisionState::Normal);
}

SelectPlayerStateIdle::~SelectPlayerStateIdle()
{
}

void SelectPlayerStateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void SelectPlayerStateIdle::Update()
{

}
