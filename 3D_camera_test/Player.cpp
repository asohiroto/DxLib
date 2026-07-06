#include "Player.h"

Player::Player() :
	_characterAngle(0),
	_handle(-1),
	_pos(VGet(0.0f, 0.0f, 0.0f)),
	_move(VGet(0.0f, 0.0f, 0.0f)),
	_movementDirection(VGet(0.0f, 0.0f, 0.0f)),
	_rotMatrix(),
	_modelRotMatrix()

{

}

Player::~Player()
{
	MV1DeleteModel(_handle);
}

void Player::Init()
{
	_handle = MV1LoadModel("data/Player.mv1");

	MV1SetPosition(_handle, _pos);
}

void Player::Update(float cameraAngle)
{
	// アナログスティックで移動
	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInput(&analogX, &analogZ, DX_INPUT_PAD1);

	// ベクトルの長さが最大で1000になる
	// プレイヤーの最大移動速度は0.01f / frame
	_move = VGet(analogX, 0.0f, -analogZ); // ベクトルの長さは0~1000

	float len = VSize(_move);
	float rate = len / 1000;

	_move = VNorm(_move);

	float speed = 10.0f * rate;
	_move = VScale(_move, speed);

	_rotMatrix = MGetRotY(cameraAngle);
	_movementDirection = VTransform(_move, _rotMatrix);

	_pos = VAdd(_pos, _movementDirection);

	MV1SetPosition(_handle, _pos);
}

void Player::Draw()
{
	MV1DrawModel(_handle);

	DrawFormatString(0, 400, 0xffffff, "player (%.2f, %.2f, %.2f)", _pos.x, _pos.y, _pos.z);
}