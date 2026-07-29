#include "Player.h"
#include "Inputs/Input.h"
#include "PlayerMove.h"
#include "Cameras/Camera.h"
#include <DxLib.h>
#include <cassert>

Player::Player() :
	_playerUnit(),
	p_Move(nullptr),
	_cameraAngle(0.0f),
	_angle(0.0f)
{
}

Player::~Player()
{
}

void Player::Init()
{
	// プレイヤーの初期設定
	_playerUnit.modelH = MV1LoadModel("data/Model_army.mv1");
	_playerUnit.pos = VGet(1000.0f, 0.0f, 0.0f);

	// 安全策
	assert(_playerUnit.modelH != -1);

	// モデルの拡大
	MV1SetScale(_playerUnit.modelH, VGet(3.0f, 3.0f, 3.0f));

	// 各ポインタの初期化
	p_Move = std::make_shared<PlayerMove>();
	p_Move->Init();
}

void Player::End()
{
}

void Player::Update(std::shared_ptr<Input> pInput, std::shared_ptr<Camera> pCamera)
{
	// カメラの向いている角度をコピー
	_cameraAngle = pCamera->GetCameraYaw();

	// プレイヤーの挙動の更新
	p_Move->Update(pInput, pCamera->GetCameraYaw());

	// プレイヤー座標の更新
	_playerUnit.pos = VAdd(_playerUnit.pos, p_Move->GetMovement());

	// モデルの向く方向を定める
	if (pInput->IsTiltingL())
	{
		_angle = atan2f(p_Move->GetMovement().x, p_Move->GetMovement().z) + DX_PI_F;
	}

	MV1SetPosition(_playerUnit.modelH, _playerUnit.pos);
	MV1SetRotationXYZ(_playerUnit.modelH, VGet(0.0f, _angle, 0.0f));
}

void Player::Draw()
{
	MV1DrawModel(_playerUnit.modelH);
}
