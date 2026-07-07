#include "Player.h"
#include"Character.h"
#include"Input.h"
#include<cassert>
#include<cmath>

Player::Player() :
	_movementY(0.0f),
	_move(VGet(0.0f, 0.0f, 0.0f)),
	_movementDirection(VGet(0.0f, 0.0f, 0.0f)),
	_rotMatrix(),
	_modelRotMatrix()
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
	assert(_modelH != -1);

	p_Input = std::make_shared<Input>();
}

void Player::Update(float cameraAngle)
{
	p_Input->Update();
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
	float speed = 10.0f * rate;

	// 移動速度を反映した移動量を決定
	_move = VScale(_move, speed);

	// カメラの回転角から行列を作成し、移動方向をカメラ基準に変換
	_rotMatrix = MGetRotY(cameraAngle);
	_movementDirection = VTransform(_move, _rotMatrix);

	_movementY -= 1.0f;

	if (p_Input->IsTrigger(PAD_INPUT_A) && _pos.y <= 0.0f)
	{
		_movementY = 10.0f;
	}

	if (VSize(_movementDirection) > 0.0f)
	{
		_angle = atan2f(_movementDirection.x, _movementDirection.z) + DX_PI_F;
	}

	_movementDirection.y = _movementY;

	MV1SetRotationXYZ(_modelH, VGet(0.0f, _angle, 0.0f));

	// 位置を更新
	_pos = VAdd(_pos, _movementDirection);

	if (_pos.y <= 0.0f)
	{
		_movementY = 0.0f;
		_pos.y = 0.0f;
	}

	MV1SetPosition(_modelH, _pos);
}

void Player::Draw()
{
	MV1DrawModel(_modelH);
}