#include "BattleAreaManager.h"
#include "BattleArea.h"
#include "StageObject.h"
#include "../ActorManager.h"
#include "../Character/Enemy/EnemyManager.h"
#include "../../../General/CSV/CSVDataLoader.h"
#include "../../../General/CSV/ActorData.h"

namespace
{
	const std::wstring kBattleAreaCSV = L"BattleArea";
}

BattleAreaManager::BattleAreaManager()
{
}

BattleAreaManager::~BattleAreaManager()
{
}

//初期化
void BattleAreaManager::Init(std::wstring stageName, std::shared_ptr<ActorManager> actorManager)
{
	//CSVを読み込む
	auto& csvLoader = CSVDataLoader::GetInstance();
	std::wstring path = stageName + std::wstring(L"/") + kBattleAreaCSV;
	auto csvDatas = csvLoader.LoadCSV(path.c_str());
	if (csvDatas.empty())return;
	//奇数なら作れない
	if (csvDatas.size() % 2 != 0)return;
	std::vector<std::shared_ptr<StageObject>> areaParts;
	//エリアの作成
	for (auto& data : csvDatas)
	{
		if (!data)continue;
		//データを変換
		auto actorData = std::make_shared<ActorData>(data);
		if (actorData->GetActorType() == ActorData::ActorType::Stage)
		{
			//エリアを構成する一部
			auto areaPoint = std::make_shared<StageObject>(actorData, actorManager);
			actorManager->Entry(std::dynamic_pointer_cast<Actor>(areaPoint));
			areaParts.emplace_back(areaPoint);
		}
		//偶数ならエリアを一つ作成
		int size = areaParts.size();
		if (size % 2 == 0 && size > 0)
		{
			//エリアを作成
			auto area = std::make_shared<BattleArea>(areaParts[0], areaParts[1]);
			m_areas.emplace_back(area);

			//パーツリセット
			areaParts.clear();
		}
	}

	//エリアの初期化処理
	for (auto& area : m_areas)
	{
		area->Init(actorManager->GetEnemyManager());
	}

	//エネミーマネージャー
	m_pEnemyManager = actorManager->GetEnemyManager();
}

void BattleAreaManager::Update(std::shared_ptr<ActorManager> actorManager)
{
	//更新
	for (auto area : m_areas)
	{
		//更新
		area->Update(actorManager);
		//エリア内で戦闘が起きているなら
		if (area->IsInArea())
		{
			if (m_pEnemyManager.expired())continue;
			//エリア外の敵の活動を停止
			m_pEnemyManager.lock()->InAreaBattle(area->GetAreaEnemys());
		}
	}
	//削除チェック
	CheckDeleteArea();
}

void BattleAreaManager::End()
{
	//終了処理
	for (auto area : m_areas)
	{
		area->End();
	}
}
void BattleAreaManager::Restart()
{
	//終了処理
	for (auto area : m_areas)
	{
		area->Init(m_pEnemyManager);
	}
}

bool BattleAreaManager::IsEndAllArea() const
{
	return m_areas.size() <= 0;
}

void BattleAreaManager::CheckDeleteArea()
{
	std::vector<std::shared_ptr<BattleArea>> deleteAreas;
	//終了したものを探す
	for (auto area : m_areas)
	{
		if (area->IsNoEnemys())
		{
			deleteAreas.emplace_back(area);
			if (m_pEnemyManager.expired())continue;
			//エリア内で戦闘終了
			m_pEnemyManager.lock()->FinishInAreaBattle();
		}
	}
	for (auto& area : deleteAreas)
	{
		area->End();
		m_areas.erase(std::remove(m_areas.begin(), m_areas.end(), area), m_areas.end());
	}
}
