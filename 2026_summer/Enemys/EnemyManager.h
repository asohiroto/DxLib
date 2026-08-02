#pragma once
#include "Bases/Character.h"
#include "Enemy.h"

class EnemyManager
{
public:
	EnemyManager();
	virtual ~EnemyManager();
	void Init();
	void End();
	void Update();
	void Draw();

private:
	// 敵クラスのポインタ
	std::shared_ptr<Enemy> p_Enemy;
	// 敵
	Character::CharacterData _enemy;
};