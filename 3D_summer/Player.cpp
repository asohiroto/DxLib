#include "Player.h"
#include"Character.h"
#include"Input.h"
#include"GameDefine.h"
#include<cassert>
#include<cmath>
#include<algorithm>

using namespace GameDefine;

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
}

void Player::Update(float cameraAngle, std::shared_ptr<Input> pInput)
{
	// コントローラーの入力値の保存先
	int analogX = 0;
	int analogZ = 0;

	analogX = pInput->GetLeftStickX();
	analogZ = pInput->GetLeftStickY();

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

	// 重力をかける
	_movementY -= GRAVITY_ACCEL;

	// 地上にいるときに、Aを押すとジャンプ
	if (pInput->IsTrigger(PAD_INPUT_A) && _pos.y <= 0.0f)
	{
		_movementY = JUMP_SPEED;
	}

	// モデルが向く方向を定める
	if (VSize(_movementDirection) > 0.0f)
	{
		_angle = atan2f(_movementDirection.x, _movementDirection.z) + DX_PI_F;
	}

	// Y軸方向への移動力を決定
	_movementDirection.y = _movementY;

	// angleのほうを向かせる
	MV1SetRotationXYZ(_modelH, VGet(0.0f, _angle, 0.0f));

	// 位置を更新
	_pos = VAdd(_pos, _movementDirection);

	// 地上にいるとき
	if (_pos.y <= 0.0f)
	{
		_movementY = 0.0f;
		_pos.y = 0.0f;
	}

	_pos.x = std::clamp(static_cast<int>(_pos.x), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));
	_pos.z = std::clamp(static_cast<int>(_pos.z), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));

	MV1SetPosition(_modelH, _pos);
}

void Player::Draw()
{
	MV1DrawModel(_modelH);
}