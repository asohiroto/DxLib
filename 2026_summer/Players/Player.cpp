#include "Player.h"
#include "Inputs/Input.h"
#include "PlayerMove.h"
#include "Cameras/Camera.h"
#include <DxLib.h>
#include <cassert>

namespace
{
	// プレイヤーの半径
	constexpr float RADIUS = 130.0f;
	// 線分の長さ
	constexpr float SEGMENT_LENGTH = 270.0f;
	// 線分を高く補正する
	constexpr float SEGMENT_HEIGHT_COR = 100.0f;
}

Player::Player() :
	_playerUnit(),
	p_Move(nullptr),
	_cameraAngle(0.0f),
	_angle(0.0f)
{
}

Player::~Player()
{
	MV1DeleteModel(_playerUnit.modelH);
}

void Player::Init()
{
	// プレイヤーのキャラクターデータの初期設定
	_playerUnit.modelH = MV1LoadModel("data/Model_army.mv1");
	_playerUnit.pos = VGet(1000.0f, 0.0f, 0.0f);
	_playerUnit.radius = RADIUS;

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

	// モデルの設定
	MV1SetPosition(_playerUnit.modelH, _playerUnit.pos);
	MV1SetRotationXYZ(_playerUnit.modelH, VGet(0.0f, _angle, 0.0f));

	// 【当たり判定用】線分の始点と終点を設定
	_playerUnit.segmentStPos = VGet(_playerUnit.pos.x, SEGMENT_HEIGHT_COR, _playerUnit.pos.z);
	_playerUnit.segmentEndPos = VGet(_playerUnit.pos.x, SEGMENT_HEIGHT_COR + SEGMENT_LENGTH, _playerUnit.pos.z);
}

void Player::Draw()
{
	MV1DrawModel(_playerUnit.modelH);
#ifdef _DEBUG
	DrawCapsule3D(_playerUnit.segmentStPos, _playerUnit.segmentEndPos, _playerUnit.radius, 16, 0xff0000, 0xff0000, false);
#endif
}
