#include "Player.h"
#include <DxLib.h>
#include <cassert>

Player::Player() :
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
	i_Player.pos = VGet(1000.0f, 0.0f, 0.0f);

	printfDx("%d", i_Player.modelH);

	assert(i_Player.modelH != -1);

	// モデルの拡大
	MV1SetScale(i_Player.modelH, VGet(3.0f, 3.0f, 3.0f));
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
