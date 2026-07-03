#include "HPUIBase.h"
#include "../../General/AssetManager.h"
#include "../../General/CharaStatus.h"
#include "../../General/Math/MyMath.h"

HPUIBase::HPUIBase(std::shared_ptr<CharaStatus> charaStatus):
	m_charaStatus(charaStatus),
	m_nowValueHandle(-1),
	m_backValueHandle(-1),
	m_damageValueHandle(-1),
	m_healValueHandle(-1),
	m_barFrameHandle(-1),
	m_damageFrame(0),
	m_damageValueRate(0.0f),
	m_healFrame(0),
	m_healValueRate(0.0f),
	m_beforeNowHP(0.0f),
	m_nowHpRate(0.0f)
{
}

HPUIBase::~HPUIBase()
{
}

void HPUIBase::UpdateDamageBar()
{
	//もし死亡したらすぐにダメージバーを現在の体力に合わせる
	if (m_charaStatus->GetNowHP() <= 0)
	{
		m_damageFrame = 0;
	}

	if (m_damageFrame > 0)
	{
		--m_damageFrame;
	}
	else
	{
		//現在の体力に合わせる
		m_damageValueRate = MathSub::Lerp(m_damageValueRate, m_charaStatus->GetHPRate(), 0.1f);
	}
}

void HPUIBase::UpdateHealBar()
{
	if (m_healFrame > 0)
	{
		--m_healFrame;
	}
	else
	{
		//回復
		m_nowHpRate = MathSub::Lerp(m_nowHpRate, m_charaStatus->GetHPRate(), 0.1f);
	}
}