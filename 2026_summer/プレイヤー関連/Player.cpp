#include "Player.h"
#include "Input.h"
#include "PlayerMove.h"
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
	// 安全策
	assert(i_Player.modelH != -1);
	// モデルの拡大
	MV1SetScale(i_Player.modelH, VGet(3.0f, 3.0f, 3.0f));
	// 各ポインタの初期化
	p_Input = std::make_shared<Input>();
	p_Input->Init();
	p_Move = std::make_shared<PlayerMove>();
	p_Move->Init();
}

void Player::End()
{
}

void Player::Update()
{
	p_Input->Update();
	p_Move->Update(p_Input);

	i_Player.pos = VAdd(i_Player.pos, p_Move->GetMove());
}

void Player::Draw()
{
	MV1SetPosition(i_Player.modelH, i_Player.pos);
	MV1DrawModel(i_Player.modelH);
	p_Input->Draw();
}
