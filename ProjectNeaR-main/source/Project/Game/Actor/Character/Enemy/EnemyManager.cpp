#include "EnemyManager.h"
#include "EnemyBase.h"
#include "../../ActorManager.h"
#include "../Player/Player.h"
#include "../../../Camera/PlayerCamera.h"
#include "../../../../General/Input.h"
#include "../../../../General/Math/MyMath.h"
#include "../../../../General/Sound/SoundManager.h"
#include "../GroupManager.h"

namespace
{
	//活動範囲
	constexpr float kActiveRange = 5000.0f;
}

EnemyManager::EnemyManager(std::weak_ptr<ActorManager> actorM):
	m_enemys(),
	m_pActorManager(actorM),
	m_isInAreaBattle(false),
	m_isBeforeAliveBoss(false)
{
	m_pGroupManager = std::make_shared<GroupManager>();
}

EnemyManager::~EnemyManager()
{
}

void EnemyManager::Entry(std::shared_ptr<EnemyBase> enemy)
{
	//すでに登録されているならしない
	auto it = std::find(m_enemys.begin(), m_enemys.end(), enemy);
	if (it != m_enemys.end())return;
	//敵を追加
	m_enemys.emplace_back(enemy);

	//グループマネージャーに追加
	std::wstring tag = enemy->GetGroupTag();
	if (!(tag == L"None" || tag == L""))
	{
		m_pGroupManager->Entry(enemy);
	}

	//ボスかどうか
	if (enemy->IsBoss())
	{
		m_bosses.emplace_back(enemy);
		m_isBeforeAliveBoss = true;
	}
}

void EnemyManager::Exit(std::shared_ptr<EnemyBase> enemy)
{
	//登録されていないならしない
	auto it = std::find(m_enemys.begin(), m_enemys.end(), enemy);
	if (it == m_enemys.end())return;
	m_enemys.erase(it);

	//グループマネージャーから解除
	std::wstring tag = enemy->GetGroupTag();
	if (!(tag == L"None" || tag == L""))
	{
		m_pGroupManager->Exit(enemy);
	}
}


void EnemyManager::Init()
{
	//バトルエリアの初期化
	m_isInAreaBattle = false;
	//グループマネージャーの初期化
	m_pGroupManager->Init();
}

void EnemyManager::Update()
{
	if (m_enemys.empty())return;
	//プレイヤーとの距離で活動範囲を判定する
	if (m_pActorManager.expired())return;
	auto actorManager = m_pActorManager.lock();
	if (actorManager->GetPlayer().expired())return;
	auto player = actorManager->GetPlayer().lock();
	Vector3 playerPos = player->GetRb()->GetNextPos();
	for (auto& enemy : m_enemys)
	{
		//活動範囲内か
		bool isActive = false;

		//エリア内で戦闘中でないなら
		if (m_isInAreaBattle)
		{
			if (!enemy->IsInArea())
			{
				//エリア外の敵は非アクティブ
				enemy->SetIsActive(false);
				continue;
			}
			enemy->SearchTarget(player);
			continue;

		}

		//距離を計算
		Vector3 enemyPos = enemy->GetRb()->GetNextPos();
		float distance = (playerPos - enemyPos).XZ().Magnitude();
		//エリア内の敵なら
		if (enemy->IsInArea())
		{
			//エリア内の敵は非アクティブ
			enemy->SetIsActive(isActive);
			continue;
		}
		//活動範囲内なら活動
		isActive = distance <= kActiveRange;
		enemy->SetIsActive(isActive);

		//プレイヤーを発見
		if (isActive)
		{
			enemy->SearchTarget(player);
		}
	}
	
#if _DEBUG
	if (Input::GetInstance().IsTrigger("EnemyDead"))
	{
		for (auto& enemy : m_enemys)
		{
			//活動中の敵
			if (enemy->IsActive())
			{
				enemy->Delete();
			}
		}
	}
#endif

	//グループマネージャーの更新
	m_pGroupManager->Update();


	//全てのボスが死亡したら
	if (m_isBeforeAliveBoss)
	{
		bool isAllBossDead = true;
		for (auto& boss : m_bosses)
		{
			//死亡条件
			if (boss.expired())continue;
			if (boss.lock()->GetCharaStatus()->IsDead())continue;

			//ここまで来たら生存中のボスがいる
			isAllBossDead = false;
		}
		if (isAllBossDead)
		{
			//BGM停止
			SoundManager::GetInstance().StopBGM();
		}
	}
	//前のフレーム存在してるかチェック
	m_isBeforeAliveBoss = !m_bosses.empty();
}

void EnemyManager::End()
{
	//アクターの終了処理
	std::list<std::shared_ptr<EnemyBase>> deleteEnemy;
	for (auto& enemy : m_enemys)
	{
		deleteEnemy.emplace_back(enemy);
	}
	for (auto& enemy : deleteEnemy)
	{
		Exit(enemy);
	}
	deleteEnemy.clear();
}

void EnemyManager::InAreaBattle(std::list<std::shared_ptr<EnemyBase>> activeEnemy)
{
	//戦闘中
	m_isInAreaBattle = true;
	//活動する敵
	for (auto& enemy : m_enemys)
	{
		//一度すべて活動停止
		enemy->SetIsActive(false);
	}
	for (auto& enemy : activeEnemy)
	{
		//活動するもののみ
		enemy->SetIsActive(true);
	}
}

void EnemyManager::FinishInAreaBattle()
{
	m_isInAreaBattle = false;
}

bool EnemyManager::IsAllDeadEnemys() const
{
	return m_enemys.empty();
}

void EnemyManager::DebugDraw()const
{
	m_pGroupManager->DebugDraw();
}
