#include "Player.h"
#include "Inputs/Input.h"
#include "PlayerMove.h"
#include "Cameras/Camera.h"
#include "Magics/MagicShot.h"
#include <DxLib.h>
#include <cassert>
#include <algorithm>

namespace
{
	// プレイヤーの半径
	constexpr float RADIUS = 130.0f;
	// 線分の長さ
	constexpr float SEGMENT_LENGTH = 270.0f;
	// 線分を高く補正する
	constexpr float SEGMENT_HEIGHT_COR = 100.0f;
	// X軸方向の移動制限
	constexpr float POS_LIMIT_X = 1000.0f;
	// Z軸方向の移動制限
	constexpr float POS_LIMIT_Z = 1000.0f;
	// モデルの回転速度
	constexpr float ROTATE_SPEED = 0.03f;
	// 入力値の補正用
	constexpr float INPUT_COR = 0.001f;

	constexpr float SHOT_RIGHT_OFFSET = 150.0f;
}

Player::Player() :
	_playerUnit(),
	p_Move(nullptr),
	_angle(0.0f),
	p_Shot(nullptr),
	_frontVec(VGet(0.0f, 0.0f, 0.0f))
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
	_playerUnit.isHit = false;
	_playerUnit.color = 0xff0000;
	_playerUnit.state = Move;

	// 安全策
	assert(_playerUnit.modelH != -1);

	// モデルの拡大
	MV1SetScale(_playerUnit.modelH, VGet(3.0f, 3.0f, 3.0f));

	// 各ポインタの初期化
	p_Move = std::make_shared<PlayerMove>();
	p_Move->Init();

	p_Shot = std::make_shared<MagicShot>();
	p_Shot->Init();
}

void Player::End()
{
}

void Player::Update(std::shared_ptr<Input> pInput, std::shared_ptr<Camera> pCamera, std::shared_ptr<MagicManager> pManager)
{
	int rx = pInput->GetRightStickX();

	// プレイヤーの挙動の更新
	p_Move->Update(pInput, pCamera->GetCameraYaw());

	// プレイヤー座標の更新
	_playerUnit.pos = VAdd(_playerUnit.pos, p_Move->GetMovement());

	// 移動制限
	_playerUnit.pos.x = std::clamp(_playerUnit.pos.x, -POS_LIMIT_X, POS_LIMIT_X);
	_playerUnit.pos.z = std::clamp(_playerUnit.pos.z, -POS_LIMIT_Z, POS_LIMIT_Z);

	// モデルの向く方向を定める
	if (pInput->IsTiltingR())
	{
		_angle += static_cast<float>(ROTATE_SPEED * std::clamp(rx * INPUT_COR, -1.0f, 1.0f));
		_frontVec = GetCameraFrontVector();
		_frontVec.y = 0.0f;
	}

	if (!pCamera->GetCameraMode())
	{
		_angle = pCamera->GetCameraYaw() + DX_PI_F;
		_frontVec = pCamera->GetEnemyDirection();
	}

	// モデルの設定
	MV1SetPosition(_playerUnit.modelH, _playerUnit.pos);
	MV1SetRotationXYZ(_playerUnit.modelH, VGet(0.0f, _angle, 0.0f));

	// 【当たり判定用】線分の始点と終点を設定
	_playerUnit.segmentStPos = VGet(_playerUnit.pos.x, SEGMENT_HEIGHT_COR, _playerUnit.pos.z);
	_playerUnit.segmentEndPos = VGet(_playerUnit.pos.x, SEGMENT_HEIGHT_COR + SEGMENT_LENGTH, _playerUnit.pos.z);

	// マジックショットの生成位置を決定
	// 正面の角度をとる
	float facing = _angle - DX_PI_F;
	// 右向きベクトルを取得
	VECTOR right = VGet(std::cosf(facing), 0.0f, -std::sinf(facing));

	VECTOR shotPos = VAdd(_playerUnit.pos, VScale(right, SHOT_RIGHT_OFFSET));

	// マジックショットが存在せず、Aが押されたらマジックショットを生成
	if (pInput->IsTrigger(PAD_INPUT_A))
		p_Shot->GenerateShot(shotPos, _frontVec, false, pManager);
}

void Player::Draw()
{
	MV1DrawModel(_playerUnit.modelH);
#ifdef _DEBUG
	DrawHitBox(_playerUnit);
	if (p_Shot->IsExist()) p_Shot->Draw();
#endif
}