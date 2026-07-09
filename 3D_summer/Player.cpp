#include "Player.h"
#include"PlayerMove.h"
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
	_modelRotMatrix(),
	_angle(0.0f),
	p_PlayerMove(nullptr)
{

}

Player::~Player()
{
	MV1DeleteModel(_modelH);
}

void Player::Init(int id)
{
	p_PlayerMove = std::make_shared<PlayerMove>();

	// プレイヤーに応じてモデルを変化
	if (id == 1)
	{
		_modelH = MV1LoadModel("data/PlayerModel.mv1");
	}
	else if (id == 2)
	{
		_modelH = MV1LoadModel("data/PlayerSubModel.mv1");
	}

	MV1SetScale(_modelH, VGet(3.0f, 3.0f, 3.0f));

	// 安全策
	assert(_modelH != -1);

	// プレイヤーのコリジョン情報を設定
	SetCollision(true, CollisionType::Capsule, 300.0f, -1);
}

void Player::Update(float cameraAngle, std::shared_ptr<Input> pInput)
{
	// コントローラーの入力値の保存
	int analogX = 0;
	int analogZ = 0;
	analogX = pInput->GetLeftStickX();
	analogZ = pInput->GetLeftStickY();

	p_PlayerMove->Move(_pos, _move, analogX, analogZ, cameraAngle, _modelH);

	MV1SetPosition(_modelH, _pos);
}

void Player::Draw()
{
	// モデルの描画
	MV1DrawModel(_modelH);
	// 当たり判定用カプセルの表示
	DrawCapsule3D(
		VAdd(_pos, VGet(0.0f, 450.0f, 0.0f)),
		VAdd(_pos, VGet(0.0f, 100.0f, 0.0f)),
		120.0f,
		16,
		0xff0000,
		0xff0000,
		false
	);
}