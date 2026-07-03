#include "GroupManager.h"
#include "../Actor.h"
#include "Enemy/EnemyBase.h"
#include "../../../General/Model.h"

GroupManager::GroupManager()
{
}

GroupManager::~GroupManager()
{
}

void GroupManager::Entry(std::shared_ptr<EnemyBase> enemy)
{
	std::wstring groupName = enemy->GetGroupTag();
	//ƒAƒNƒ^[‚ğ’Ç‰Á
	m_groupEnemies[groupName].emplace_back(enemy);
	//‚Ü‚¾“o˜^‚³‚ê‚Ä‚¢‚È‚¢ƒOƒ‹[ƒv–¼‚È‚ç
	auto it = std::find(m_groupNames.begin(), m_groupNames.end(), groupName);
	if (it == m_groupNames.end())
	{
		//ƒOƒ‹[ƒv–¼“o˜^
		m_groupNames.emplace_back(groupName);
		//UŒ‚Œ ‚ğ‚½‚¹‚é
		TakeTurnCanAttack(enemy, groupName);
#if _DEBUG
		//ƒfƒoƒbƒO—p
#endif
		m_groupColor[groupName] = GetColor(MyMath::GetRand(0, 255), MyMath::GetRand(0, 255), MyMath::GetRand(0, 255));
	}
	else
	{
		//UŒ‚Œ ‚ğ‚È‚­‚·
		enemy->SetCanAttack(false);
	}
}

void GroupManager::Exit(std::shared_ptr<EnemyBase> actor)
{
	std::wstring groupName = actor->GetGroupTag();

	//“o˜^‚³‚ê‚Ä‚¢‚é‚È‚ç
	auto it = std::find(m_groupEnemies[groupName].begin(), m_groupEnemies[groupName].end(), actor);
	if (it == m_groupEnemies[groupName].end())return;

	//‚à‚µUŒ‚Œ ‚ğ‚Á‚Ä‚¢‚é‚È‚ç
	if (actor->CanAttack())
	{
		auto nextIt = it;
		++nextIt;
		//Ÿ‚Ì“G‚É“n‚·
		if (nextIt != m_groupEnemies[groupName].end())
		{
			//UŒ‚Œ ‚ğ‚½‚¹‚é
			TakeTurnCanAttack(*nextIt, groupName);
		}
		else
		{
			//Ÿ‚Ì“G‚ª‚¢‚È‚¢‚È‚çæ“ª‚É“n‚·
			TakeTurnCanAttack(m_groupEnemies[groupName].front(), groupName);
		}
	}

	//“o˜^‰ğœ
	m_groupEnemies[groupName].remove(actor);
}

void GroupManager::Init()
{
	m_groupEnemies = {};
	m_canAttackEnemiesID = {};
	m_groupNames = {};
}

void GroupManager::Update()
{
	//“o˜^‚³‚ê‚½ƒOƒ‹[ƒv–¼‚©‚çŠeƒOƒ‹[ƒv‚ğŒ©‚é
	for (auto& groupName : m_groupNames)
	{
		//‚à‚µ‚àƒOƒ‹[ƒv“à‚Å’N‚àƒ^[ƒQƒbƒg‚ğ”­Œ©‚µ‚Ä‚¢‚È‚¢ê‡‚Í
		//‰ñ‚·•K—v‚Í‚È‚¢‚Ì‚Åˆ—‚ğ‚µ‚È‚¢
		bool isGroupSearch = false;
		for (auto& enemy : m_groupEnemies[groupName])
		{
			//”­Œ©‚Å‚«‚Ä‚¢‚é‚È‚ç
			if (enemy->GetTargetInfo().m_isFound)
			{
				isGroupSearch = true;
				break;
			}
		}
		if (!isGroupSearch)continue;

		//UŒ‚Œ ‚ğŸ‚É‰ñ‚·‚©‚Ç‚¤‚©
		bool isTurnCanAttack = false;
		//‰ñ‚¹‚½‚©
		bool isTurnedCanAttack = false;
		//ƒOƒ‹[ƒv“à‚ÅUŒ‚Œ ‚ğ‰ñ‚·
		for (auto& enemy : m_groupEnemies[groupName])
		{
			//“G‚ÌID
			int enemyId = enemy->GetID();
			//UŒ‚Œ ‚ğ‚Â“G‚ÌID
			int canAttackId = m_canAttackEnemiesID[groupName];
			//UŒ‚Œ ‚ª‚ ‚é‚©
			bool canAttack = enemy->CanAttack();
			//‚à‚µ‚àƒvƒŒƒCƒ„[‚ğ”­Œ©‚Å‚«‚Ä‚¢‚È‚¢“GƒLƒƒƒ‰ƒNƒ^[‚ÉUŒ‚Œ ‚ªˆÚ‚Á‚½ê‡
			//‚»‚ÌƒLƒƒƒ‰‚ªUŒ‚Œ ‚ğ•Û‚µ‘±‚¯‚é‚Ì‚Å”­Œ©‚µ‚Ä‚È‚¢ê‡‚Í÷‚é
			if (enemyId == canAttackId && canAttack && !enemy->GetTargetInfo().m_isFound)
			{
				//UŒ‚Œ ‚ª‚È‚­‚È‚é
				enemy->SetCanAttack(false);
			}
			//‰ñ‚³‚È‚¢
			if (!isTurnCanAttack)
			{
				//ID‚©‚çUŒ‚Œ Š—LÒ‚©‚ÂUŒ‚Œ ‚ª‚È‚­‚È‚Á‚Ä‚¢‚é‚È‚ç
				if (enemyId == canAttackId && !canAttack)
				{
					//Ÿ‚Ì“G‚É“n‚·
					isTurnCanAttack = true;
				}
			}
			//‰ñ‚·
			else
			{
				//“n‚¹‚½
				isTurnedCanAttack = true;
				TakeTurnCanAttack(enemy, groupName);
				break;
			}
		}
		//“n‚·‚Â‚à‚è‚¾‚Á‚½‚ªŸ‚Ì“G‚ª‚¢‚È‚­‚Ä“n‚¹‚È‚©‚Á‚½‚Æ‚«
		if (isTurnCanAttack && !isTurnedCanAttack)
		{
			//æ“ª‚É“n‚·
			TakeTurnCanAttack(m_groupEnemies[groupName].front(), groupName);
		}
	}
}

void GroupManager::End()
{
	//“G‚Ì“o˜^‰ğœ
	std::list<std::shared_ptr<EnemyBase>> deleteEnemy;
	for (auto& groupName : m_groupNames)
	{
		for (auto& enemy : m_groupEnemies[groupName])
		{
			deleteEnemy.emplace_back(enemy);
		}
	}
	for (auto& enemy : deleteEnemy)
	{
		Exit(enemy);
	}
	deleteEnemy.clear();
	//‚·‚×‚Ä“o˜^‰ğœ
	m_groupEnemies.clear();
	m_canAttackEnemiesID.clear();
}

void GroupManager::TakeTurnCanAttack(std::shared_ptr<EnemyBase> enemy, std::wstring& groupName)
{
	//UŒ‚Œ ‚ğ‚½‚¹‚é
	enemy->SetCanAttack(true);
	//ID“o˜^
	m_canAttackEnemiesID[groupName] = enemy->GetID();
}


void GroupManager::DebugDraw()const
{
	for (auto& groupName : m_groupNames)
	{
		for (auto& enemy : m_groupEnemies.at(groupName))
		{
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 230);
			//F‚Å‚Ç‚ÌƒOƒ‹[ƒv‚©‚ğ”»•Ê
			DrawSphere3D(enemy->GetCenterPos().ToDxLibVector(), enemy->GetRadius(), 16, m_groupColor.at(groupName), m_groupColor.at(groupName), true);
			//UŒ‚Œ ‚ğ‚Á‚Ä‚éê‡
			if (enemy->CanAttack())
			{
				DrawSphere3D(enemy->GetHeadPos().ToDxLibVector(), enemy->GetRadius(), 16, 0xff0000, 0xff0000, true);
			}
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		}
	}
}
