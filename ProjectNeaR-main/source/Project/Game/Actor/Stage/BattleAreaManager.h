#pragma once
#include <memory>
#include <string>
#include <vector>

class ActorManager;
class BattleArea;
class EnemyManager;
class BattleAreaManager
{
public:
	BattleAreaManager();
	~BattleAreaManager();
	//初期化
	void Init(std::wstring stageName,std::shared_ptr<ActorManager> actorManager);
	//更新
	void Update(std::shared_ptr<ActorManager> actorManager);
	//終了処理
	void End();
	//再スタート
	void Restart();

	//すべてのエリアを突破した
	bool IsEndAllArea()const;
private:
	//エリア
	std::vector<std::shared_ptr<BattleArea>> m_areas;
	//エネミーマネージャー
	std::weak_ptr<EnemyManager> m_pEnemyManager;
private:
	//終了したエリアの削除
	void CheckDeleteArea();
};

