#pragma once
#include <map>
#include <list>
#include <string>
#include <memory>
#include <DxLib.h>
class EnemyBase;
//グループ内での処理を行うクラス
class GroupManager
{
public:
	GroupManager();
	~GroupManager();
	void Entry(std::shared_ptr<EnemyBase> enemy);
	void Exit(std::shared_ptr<EnemyBase> enemy);
	void Init();
	void Update();
	void End();

	//デバッグ用
	void DebugDraw() const;
private:
	//各グループの敵
	std::map<std::wstring, std::list<std::shared_ptr<EnemyBase>>> m_groupEnemies;
	//各グループの攻撃権を持つ敵のID
	std::map<std::wstring, int> m_canAttackEnemiesID;
	//各グループ名
	std::list<std::wstring> m_groupNames;
	//攻撃の権利を回す
	void TakeTurnCanAttack(std::shared_ptr<EnemyBase> enemy, std::wstring& groupName);


	//デバッグ用
	//グループカラー
	std::map<std::wstring, unsigned int> m_groupColor;
};

