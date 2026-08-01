#pragma once
#include "Bases/Character.h"
#include <DxLib.h>
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

private:
	// キャラクター同士の当たり判定
	bool CharacterHitCheck(Character::CharacterData player, Character::CharacterData enemy);
	// キャラクター同士が当たった瞬間の取得
	bool IsTouch() { return (_isTouchNow && !_isTouchLast); }
	// キャラクター同士が当たっている時の取得
	bool IsTouching() { return (_isTouchNow && _isTouchLast); }
	// キャラクター同士が離れた瞬間の取得
	bool IsSeparate() { return (!_isTouchNow && _isTouchLast); }
	// 押し戻す方向と距離を設定する関数
	void SetPullBack(VECTOR pullback) { _pullBack = pullback; }

private:
	// このフレームで接触しているか
	bool _isTouchNow;
	// ひとつ前のフレームで接触しているか
	bool _isTouchLast;
	// 押し戻す量と方向
	VECTOR _pullBack;
};