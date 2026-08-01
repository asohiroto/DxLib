#pragma once
#include "Bases/MagicBase.h"
#include <vector>

class MagicManager
{
private:
	// 生成された魔法を格納するリストの型
	using MagicList = std::vector<MagicBase::MagicData>;

public:
	MagicManager();
	virtual ~MagicManager();
	void Init();
	void End();
	void Update();
	void Draw();
	// リストに魔法を登録する関数
	void EntryList(MagicBase::MagicData data);
	// プレイヤーの使用した魔法のゲッター
	MagicList GetPlayerList() { return _playerMagics; }
	// 敵が使用した魔法のゲッター
	MagicList GetEnemyList() { return _enemyMagics; }

private:
	// プレイヤーが使用した魔法のリスト
	MagicList _playerMagics;
	// 敵が使用した魔法のリスト
	MagicList _enemyMagics;
};