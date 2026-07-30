#pragma once

class Player;
class Enemy;

class Collision
{
public:
	Collision();
	virtual ~Collision();
	void Init();
	void End();
	void Update();
	void Draw();

	// キャラクター同士の当たり判定
	void CharacterHitCheck(std::shared_ptr<Player> pPlayer, std::shared_ptr<Enemy> pEnemy);
};