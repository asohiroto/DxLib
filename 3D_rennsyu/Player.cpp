#include "player.h"

#include<DxLib.h>
#include<cassert>

namespace
{
	const char* kModelPath = "data/Player.mv1";
}

Player::Player() :
	_modelH(-1),
	_animH(-1),
	_pos(VGet(0.0f, 0.0f, 0.0f)),
	_move()
{

}

Player::~Player()
{
	MV1DeleteModel(_modelH);
}

void Player::Init()
{
	_modelH = MV1LoadModel(kModelPath);
	assert(_modelH != -1);

	MV1SetPosition(_modelH, _pos);
}

void Player::Update()
{
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInput(&analogX, &analogZ, DX_INPUT_PAD1);

	_move = VGet(analogX, 0, -analogZ);

	float len = VSize(_move);
	float rate = len / 1000;

	_move = VNorm(_move);

	float speed = 10.0f * rate;

	_move = VScale(_move, speed);

	_pos = VAdd(_pos, _move);

	MV1SetPosition(_modelH, _pos);
}

void Player::Draw() const
{
	MV1DrawModel(_modelH);
}