#pragma once
#include <DxLib.h>
#include <memory>

class Enemy;
class EnemyMove;

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();
	void Init();
	void End();
	void Update(std::shared_ptr<Enemy> pEnemy, VECTOR playerPos);
	void Draw();

private:

	// エネミーの移動状態時処理
	std::shared_ptr<EnemyMove> p_Move;
};