#pragma once
#include "Bases/MagicBase.h"
#include <vector>
#include <memory>

class MagicMove;
class MagicCollision;

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
	void Update(VECTOR playerPos, VECTOR enemyPos);
	void Draw();

	// リストに魔法を登録する関数
	void EntryList(MagicBase::MagicData data);
	// プレイヤーの使用した魔法のゲッター
	MagicList& GetPlayerList() { return _playerMagics; }
	// 敵が使用した魔法のゲッター
	MagicList& GetEnemyList() { return _enemyMagics; }
	// リストから条件を満たした魔法を消す関数
	void RemoveList();
	// 敵をロックオンする
	void LockOn() { _enemyLock = true; }
	// ロックオンしているか
	bool IsLockOn() const { return _enemyLock; }
	// 敵座標のゲッター
	VECTOR GetEnePos() const { return _enePos; }
private:
	// 魔法の当たり判定を描画する
	void DrawMagicHitBox(MagicBase::MagicData data);

private:
	// プレイヤーが使用した魔法のリスト
	MagicList _playerMagics;
	// 敵が使用した魔法のリスト
	MagicList _enemyMagics;
	// マジックムーブの共有ポインタ
	std::shared_ptr<MagicMove> p_MagicMove;
	// 敵をロックオンしているか
	bool _enemyLock;
	// 敵の座標
	VECTOR _enePos;
};