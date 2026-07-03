#pragma once
#include <memory>
#include <list>
class EnemyBase;
class ActorManager;
class GroupManager;
class EnemyManager : 
	public std::enable_shared_from_this<EnemyManager>
{
public:
	EnemyManager(std::weak_ptr<ActorManager> actorM);
	~EnemyManager();
	//登録
	void Entry(std::shared_ptr<EnemyBase> enemy);
	//解除
	void Exit(std::shared_ptr<EnemyBase> enemy);
	//初期化
	void Init();
	//更新
	void Update();
	//終了処理
	void End();

	//敵のリスト取得
	const std::list<std::shared_ptr<EnemyBase>>& GetEnemys() const { return m_enemys; }

	//エリア内で戦闘が起きたときの処理
	void InAreaBattle(std::list<std::shared_ptr<EnemyBase>> activeEnemy);
	//エリア内での戦闘終了
	void FinishInAreaBattle();

	//すべての敵が倒された
	bool IsAllDeadEnemys()const;

	//デバッグ描画
	void DebugDraw()const;
private:
	//敵
	std::list<std::shared_ptr<EnemyBase>> m_enemys;
	//アクターマネージャーの参照
	std::weak_ptr<ActorManager> m_pActorManager;
	//グループマネージャー
	std::shared_ptr<GroupManager> m_pGroupManager;
	//エリア内で戦闘中
	bool m_isInAreaBattle;
	//ボス
	std::list<std::weak_ptr<EnemyBase>> m_bosses;
	//ボスがいるか
	bool m_isBeforeAliveBoss;
};

