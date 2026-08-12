#pragma once
#include <DxLib.h>
#include <memory>

class Enemy;
class EnemyMove;
class MagicBeam;
class MagicManager;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();
	void Init();
	void End();
	void Update(VECTOR playerPos, std::shared_ptr<MagicManager> pMManager);
	void Draw();
	// 座標のゲッター
	VECTOR GetEnemyPos() const;
	// エネミーのポインタを渡す
	std::shared_ptr<Enemy> GetEnemyPointer() const;

private:
	// エネミーの共有ポインタ
	std::shared_ptr<Enemy> p_Enemy;
	// エネミーの移動状態時処理
	std::shared_ptr<EnemyMove> p_Move;
	// マジックビームの共有ポインタ
	std::shared_ptr<MagicBeam> p_Beam;
};