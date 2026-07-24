#include "Player.h"
#include <DxLib.h>

Player::Player():
	i_Player()
{
}

Player::~Player()
{
}

void Player::Init()
{
	// プレイヤーの初期設定
	i_Player.modelH = MV1LoadModel("data/Model_army.mv1");
	i_Player.pos = VGet(0.0f, 0.0f, 0.0f);

	// プレイヤーの初期位置
	MV1SetPosition(i_Player.modelH, i_Player.pos);
}

void Player::End()
{
}

void Player::Update()
{
}

void Player::Draw()
{
	MV1DrawModel(i_Player.modelH);
}
