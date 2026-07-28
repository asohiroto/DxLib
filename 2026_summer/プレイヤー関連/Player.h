#pragma once
#include "Character.h"
#include<memory>

class Input;
class PlayerMove;

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
	// インプットクラスの共有ポインタ
	std::shared_ptr<Input> p_Input;
	// 移動用クラスの共有ポインタ
	std::shared_ptr<PlayerMove> p_Move;
};