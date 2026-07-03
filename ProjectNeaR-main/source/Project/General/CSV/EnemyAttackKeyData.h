#pragma once
#include "CSVData.h"
#include "../CharaStatus.h"
#include <memory>
class EnemyAttackKeyData :
	public CSVData
{
public:
	EnemyAttackKeyData();
	EnemyAttackKeyData(std::shared_ptr<CSVData> data);
	~EnemyAttackKeyData();
	
	//UŒ‚‚Ì‹——£
	enum class AttackRangeType
	{
		Melee,		//‹ßÚUŒ‚
		LongRange,	//‰“‹——£UŒ‚
	};
	//UŒ‚‚Ì‹——£
	AttackRangeType	GetAttackRangeType()const { return m_attackRangeType; };
	//UŒ‚‚ÌƒL[–¼
	std::wstring	GetAttackKeyName()const { return m_attackKeyName; };
private:
	//•ÏŠ·
	void Conversion() override;

	AttackRangeType	m_attackRangeType;	//UŒ‚‚Ì‹——£
	//UŒ‚‚ÌƒL[–¼
	std::wstring	m_attackKeyName;	//UŒ‚‚ÌƒL[–¼
};

