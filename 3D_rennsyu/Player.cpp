#include "player.h"
#include"Camera.h"
#include"Input.h"
#include<DxLib.h>
#include<cassert>

namespace
{
	const char* kModelPath = "data/Player.mv1";
	constexpr float kRadius = 100.0f;
}

Player::Player() :
	_modelH(-1),
	_animH(-1),
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

	SetCollision(true, CollisionType::Sphere, kRadius);

	_vec.x = 0.0f;
	_vec.z = 0.0f;

	SetPos(VGet(0.0f, 300.0f, 0.0f));

	MV1SetPosition(_modelH, _pos);
}

void Player::Update()
{
	auto& input = Input::GetInstance();

	int analogX = 0;
	int analogZ = 0;

	GetJoypadAnalogInput(&analogX, &analogZ, DX_INPUT_PAD1);

	_move = VGet(analogX, 0, -analogZ);

	float len = VSize(_move);
	float rate = len / 1000;

	_move = VNorm(_move);

	auto& camera = Camera::GetInstance();
	float yaw = camera.GetCameraYaw();

	float sinY = sinf(yaw);
	float cosY = cosf(yaw);

	float originalX = _move.x;
	float originalZ = _move.z;

	_move.x = originalX * cosY + originalZ * sinY;
	_move.z = -originalX * sinY + originalZ * cosY;

	if (VSquareSize(_move) > 0.0f)
	{
		_move = VNorm(_move);
	}

	float speed = 10.0f * rate;
	_move = VScale(_move, speed);

	if (_isGrounded == false)
	{
		_vec.y -= 0.5f;

	}

	if (input.IsTrigger(PAD_INPUT_A) && _isGrounded)
	{
		_vec.y = 20.0f;
		_isGrounded = false;
	}

	_vec.x = _move.x;
	_vec.z = _move.z;

	_pos = VAdd(_pos, _vec);

	VECTOR pos = _pos;
	pos.y -= _radius;

	MV1SetPosition(_modelH, pos);

	if (VSquareSize(_vec) > 0.0f)
	{
		float angle = atan2f(_vec.x, _vec.z) + DX_PI_F;

		MV1SetRotationXYZ(_modelH, VGet(0.0f, angle, 0.0f));
	}
}

void Player::Draw() const
{
	DrawSphere3D(_pos, _radius, 16, 0xff0000, 0xff0000, false);
	MV1DrawModel(_modelH);
}