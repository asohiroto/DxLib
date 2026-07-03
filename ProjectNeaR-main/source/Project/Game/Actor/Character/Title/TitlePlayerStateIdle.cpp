#include "TitlePlayerStateIdle.h"
#include "TitlePlayer.h"
#include "../../../../General/Model.h"

namespace
{
	//アニメーション
	const std::wstring kIdle = L"TitleIdle";
}

TitlePlayerStateIdle::TitlePlayerStateIdle(std::weak_ptr<Actor> player) :
	CharacterStateBase(player,false)
{
	if (m_pOwner.expired())return;
	auto owner = std::dynamic_pointer_cast<TitlePlayer>(m_pOwner.lock());

	//アニメーション
	owner->GetModel()->SetAnim(owner->GetAnim(kIdle).c_str(), true);
	owner->SetCollState(CollisionState::Normal);
}

TitlePlayerStateIdle::~TitlePlayerStateIdle()
{
	
}

void TitlePlayerStateIdle::Init()
{
	//次の状態を自分の状態を入れる
	ChangeState(shared_from_this());
}

void TitlePlayerStateIdle::Update()
{

}
