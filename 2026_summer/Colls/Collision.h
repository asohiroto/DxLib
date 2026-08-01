#pragma once
#include <memory>

class Player;
class Enemy;

class Collision
{
public:
	Collision();
	virtual ~Collision();
	void Init();
	void End();
	void Update(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy> pEnemy);
	void Draw();

	// キャラクター同士の当たり判定
	bool CharacterHitCheck(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy> pEnemy);

};