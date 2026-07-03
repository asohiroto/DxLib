#pragma once
#include <memory>
#include "../Game/Actor/Character/CharacterBase.h"
//ターゲット情報
struct TargetInfo
{
	TargetInfo() :
		m_isFound(false),
		m_pTarget()
	{
	}
	//発見したかどうか
	bool m_isFound;
	//参照
	std::weak_ptr<CharacterBase> m_pTarget;
};