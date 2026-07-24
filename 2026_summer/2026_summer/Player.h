#pragma once
#include "Character.h"
#include<memory>

class Player :
	public Character
{
public:
	Player();
	virtual ~Player() override;
	void Init() override;
	void End()override;
	void Update() override;
	void Draw() override;

	// プレイヤー座標のゲッター
	VECTOR GetPos() const { return i_Player.pos; }

private:
	 // プレイヤーのインスタンス
	CharacterData i_Player;
};

