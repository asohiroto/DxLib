#include "Player.h"
#include"Character.h"
#include<cassert>

Player::Player() :
	_move(VGet(0.0f, 0.0f, 0.0f))
{

}

Player::~Player()
{
	MV1DeleteModel(_modelH);
}

void Player::Init()
{
	_modelH = MV1LoadModel("data/Player.mv1");
	// 安全策
	assert(!_modelH);
}

void Player::Update()
{
	// コントローラーの入力値の保存先
	int analogX = 0;
	int analogZ = 0;

	// スティックの入力検知
	GetJoypadAnalogInput(&analogX, &analogZ, DX_INPUT_PAD1);

	// 移動量に代入
	_move = VGet(analogX, 0.0f, -analogZ);

	// スティックの傾き具合を計算
	float len = VSize(_move);
	// 0.0～1.0の割合に変換
	float rate = len / 1000;

	// 正規化
	_move = VNorm(_move);

	// 移動速度（最大で10.0f）
	float speed = 10.0f / rate;

	// 最終的な移動量を決定
	_move = VScale(_move, speed);
	// 位置を更新
	_pos = VAdd(_pos, _move);

	MV1SetPosition(_modelH, _pos);
}

void Player::Draw()
{
	MV1DrawModel(_modelH);
}