#include "Player.h"
#include "Inputs/Input.h"
#include "PlayerMove.h"
#include "PlayerDodge.h"
#include "Cameras/Camera.h"
#include "Magics/MagicShot.h"
#include "Magics/MagicMissile.h"
#include "Magics/MagicFury.h"
#include "Magics/MagicBeam.h"
#include "Magics/MagicManager.h"
#include "Anims/AnimManager.h"
#include <DxLib.h>
#include <EffekseerForDXLib.h>
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
	constexpr float POS_LIMIT_X = 3000.0f;
	// Z軸方向の移動制限
	constexpr float POS_LIMIT_Z = 3000.0f;
	// モデルの回転速度
	constexpr float ROTATE_SPEED = 0.03f;
	// 入力値の補正用
	constexpr float INPUT_COR = 0.001f;
	// 魔法を撃つ位置を右側に補正する
	constexpr float SHOT_RIGHT_OFFSET = 150.0f;
	// 魔法の切り替わりフレーム
	constexpr int SHOT_SWITCH = 30;
	// 最大体力
	constexpr int MAX_HP = 1000;
	// 最大魔力
	constexpr float MAX_MP = 500.0f;
	// 魔力自動回復量（1フレーム）
	constexpr float AUTO_MP_HEAL = 0.25f;
	// やられフレーム数
	constexpr int DAMAGED_FRAME = 10;
	// ダメージ判定の色
	constexpr int DAMAGED_COLOR = 0x00ff00;
	// 通常時の色
	constexpr int NORM_COLOR = 0xff0000;
	// マジックサークルの再生位置を前方に補正
	constexpr float CIRCLE_FRONT_OFFSET = 30.0f;
	// マジックサークルの再生位置の高さを修正
	constexpr float CIRCLE_HEIGHT_OFFSET = 300.0f;
	// 通常時のジャスト回避判定の半径（存在しないため０）
	constexpr float NORM_JUST_RADIUS = 0.0f;
	// ジャスト回避判定の半径
	constexpr float JUST_DODGE_RADIUS = 300.0f;
	// ジャスト回避によって得ることのできる必殺技のチャージ量
	constexpr float ULT_CHARGE_AMOUNT = 20;
	// ジャスト回避によって得られるMPの量
	constexpr float MP_GAIN_AMOUNT = 100.0f;
	// ジャスト回避によって得られるHPの量
	constexpr int HP_HEAL_AMOUNT = 50;
	// 必殺技チャージの最大量
	constexpr float MAX_ULT_CHARGE = 100.0f;
}

Player::Player() :
	_playerUnit(),
	p_Move(nullptr),
	p_Dodge(nullptr),
	_angle(0.0f),
	p_Shot(nullptr),
	p_Missile(nullptr),
	p_Fury(nullptr),
	p_Beam(nullptr),
	p_AManager(nullptr),
	_frontVec(VGet(0.0f, 0.0f, 0.0f)),
	_pressFrame(0),
	_damagedCount(0),
	_magicCircleH(-1),
	_circlePlayingH(-1),
	_targetPlayingH(-1),
	_isDodge(false)
{
}

Player::~Player()
{
	StopEffekseer3DEffect(_circlePlayingH);
}

void Player::Init(int handle, EffectHandles playerMagics)
{
	// プレイヤーのキャラクターデータの初期設定
	_playerUnit.modelH = handle;
	_playerUnit.pos = VGet(1000.0f, 0.0f, 1000.0f);
	_playerUnit.radius = RADIUS;
	_playerUnit.isHit = false;
	_playerUnit.color = NORM_COLOR;
	_playerUnit.nowState = CharacterState::Wait;
	// プレイヤーのステータスの初期設定
	_playerUnit.maxHp = MAX_HP;
	_playerUnit.hp = _playerUnit.maxHp;
	_playerUnit.maxMp = MAX_MP;
	_playerUnit.mp = _playerUnit.maxMp;
	_playerUnit.justRadius = NORM_JUST_RADIUS;
	_playerUnit.ultCharge = 0.0f;
	_playerUnit.maxUltCharge = MAX_ULT_CHARGE;

	// 安全策
	assert(_playerUnit.modelH != -1);

	// モデルの拡大
	MV1SetScale(_playerUnit.modelH, VGet(2.5f, 2.5f, 2.5f));

	// 各ポインタの初期化
	p_Move = std::make_shared<PlayerMove>();
	p_Move->Init();
	p_Dodge = std::make_shared<PlayerDodge>();
	p_Dodge->Init();
	p_Shot = std::make_shared<MagicShot>();
	p_Shot->Init();
	p_Shot->SetMagicShotH(playerMagics.shotHandle);
	p_Missile = std::make_shared<MagicMissile>();
	p_Missile->Init();
	p_Missile->SetMagicMissileH(playerMagics.missileHandle);
	p_Fury = std::make_shared<MagicFury>();
	p_Fury->Init();
	p_Fury->SetMagicFuryH(playerMagics.furyHandle);
	p_Beam = std::make_shared<MagicBeam>();
	p_Beam->Init();
	p_Beam->SetMagicBeamH(playerMagics.beamHandle);
	p_AManager = std::make_shared<AnimManager>();
	p_AManager->Init(_playerUnit.modelH);

	_magicCircleH = playerMagics.circleHandle;

	p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
}

void Player::End()
{

}

void Player::Update(std::shared_ptr<Input> pInput, std::shared_ptr<Camera> pCamera, std::shared_ptr<MagicManager> pManager)
{
	_pressFrame++;
	_damagedCount++;

	// 魔力を最大値まで自動回復
	_playerUnit.mp += AUTO_MP_HEAL;
	if (_playerUnit.mp >= _playerUnit.maxMp)
		_playerUnit.mp = _playerUnit.maxMp;

	int rx = pInput->GetRightStickX();

	// 回避中ならジャスト回避判定の半径を有効化する
	if (p_Dodge->IsDodge())
		_playerUnit.justRadius = JUST_DODGE_RADIUS;
	else
		_playerUnit.justRadius = NORM_JUST_RADIUS;

	// 埋まり防止用
	if (_playerUnit.pos.y <= 0.0f) _playerUnit.pos.y = 0.0f;

	if (_playerUnit.isHit)
	{
		if (_damagedCount <= DAMAGED_FRAME)
			_playerUnit.color = DAMAGED_COLOR;
		else if (_damagedCount > DAMAGED_FRAME)
		{
			_playerUnit.color = NORM_COLOR;
			_playerUnit.isHit = false;
		}
	}

	// プレイヤーの挙動の更新
	p_Move->Update(pInput, pCamera->GetCameraYaw());
	p_Dodge->Update(pInput, pCamera->GetCameraYaw());

	// プレイヤー座標の更新
	_playerUnit.pos = VAdd(_playerUnit.pos, p_Move->GetMovement());
	_playerUnit.pos = VAdd(_playerUnit.pos, p_Dodge->GetDodgePos());

	// 回避を行っているかを更新する
	_isDodge = p_Dodge->IsDodge();

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

	if (pCamera->GetCameraMode())
	{
		_angle = pCamera->GetCameraYaw();
		_frontVec = pCamera->GetEnemyDirection();
	}

	// モデルの設定
	MV1SetPosition(_playerUnit.modelH, _playerUnit.pos);
	MV1SetRotationXYZ(_playerUnit.modelH, VGet(0.0f, _angle + DX_PI_F, 0.0f));

	// 【当たり判定用】線分の始点と終点を設定
	_playerUnit.segmentStPos = VGet(_playerUnit.pos.x, SEGMENT_HEIGHT_COR, _playerUnit.pos.z);
	_playerUnit.segmentEndPos = VGet(_playerUnit.pos.x, SEGMENT_HEIGHT_COR + SEGMENT_LENGTH, _playerUnit.pos.z);

	// マジックショットの生成位置を決定
	// 正面の角度をとる
	float facing = _angle;
	// 右向きベクトルを取得
	VECTOR right = VGet(std::cosf(facing), 0.0f, -std::sinf(facing));
	// 魔法を撃つ位置
	VECTOR shotPos = VAdd(_playerUnit.pos, VScale(right, SHOT_RIGHT_OFFSET));
	// マジックサークルの再生位置
	VECTOR circlePos = VAdd(VAdd(_playerUnit.pos, VScale(_frontVec, CIRCLE_FRONT_OFFSET)), VScale(right, SHOT_RIGHT_OFFSET));

	// RBを推している時間を計測
	if (pInput->IsTrigger(PAD_INPUT_6))
	{
		_pressFrame = 0;

		// 前の魔法陣が残っていれば先に止める
		if (_circlePlayingH != -1) StopEffekseer3DEffect(_circlePlayingH);

		_circlePlayingH = PlayEffekseer3DEffect(_magicCircleH);
		SetPosPlayingEffekseer3DEffect(_circlePlayingH, circlePos.x, circlePos.y + CIRCLE_HEIGHT_OFFSET, circlePos.z);
	}

	// マジックミサイル生成になればカメラがロックオン
	if (pInput->IsPress(PAD_INPUT_6))
	{
		SetPosPlayingEffekseer3DEffect(_circlePlayingH, circlePos.x, circlePos.y + CIRCLE_HEIGHT_OFFSET, circlePos.z);

		SetRotationPlayingEffekseer3DEffect(_circlePlayingH, 0.0f, facing, 0.0f);

		if (_pressFrame < SHOT_SWITCH) pCamera->SetCameraMode(false);
		else pCamera->SetCameraMode(true);
	}

	// 離したときの時間で生成する魔法を切り替え
	if (pInput->IsRelease(PAD_INPUT_6))
	{
		if (_pressFrame < SHOT_SWITCH)
		{
			float remainMp = _playerUnit.mp - p_Shot->GetUseMp();
			if (remainMp >= 0)
			{
				_playerUnit.mp = remainMp;
				VECTOR camFront = VGet(GetCameraFrontVector().x, 0.0f, GetCameraFrontVector().z);
				_frontVec = GetCameraFrontVector();
				p_Shot->GenerateShot(shotPos, _frontVec, false, pManager);
				_playerUnit.nowState = CharacterState::Shot;
				p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
			}
		}
		else
		{
			float remainMp = _playerUnit.mp - p_Missile->GetUseMp();
			if (remainMp >= 0)
			{
				_playerUnit.mp = remainMp;
				p_Missile->GenerateMissile(shotPos, _frontVec, false, pManager);
				_playerUnit.nowState = CharacterState::Missile;
				p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
			}
		}

		StopEffekseer3DEffect(_circlePlayingH);

		pCamera->SetCameraMode(false);
	}

	if (_playerUnit.ultCharge >= _playerUnit.maxUltCharge)
	{
		if (pInput->IsTrigger(PAD_INPUT_8))
		{
			p_Fury->GenerateFury(pManager->GetEnePos(), VGet(0.0f, -1.0f, 0.0f), false, pManager);
			_playerUnit.nowState = CharacterState::Fury;
			p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
			_playerUnit.ultCharge = 0;
		}
	}

	if (_playerUnit.ultCharge >= _playerUnit.maxUltCharge / 2)
	{
		if (pInput->IsTrigger(PAD_INPUT_7))
		{
			p_Beam->GenerateBeam(shotPos, _frontVec, false, pManager);
			_playerUnit.nowState = CharacterState::Beam;
			p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
			_playerUnit.ultCharge -= (_playerUnit.maxUltCharge / 2);
		}
	}


	// ステートの更新
	UpdateState(pInput);

	// アニメションの更新
	p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
	p_AManager->Update();
}

void Player::Draw()
{
	MV1DrawModel(_playerUnit.modelH);

	p_Dodge->Draw();
	p_AManager->Draw();

#ifdef _DEBUG
	DrawHitBox(_playerUnit);
	if (p_Shot->IsExist()) p_Shot->Draw();

	DrawFormatString(0, 60, 0xffffff, "NowHp : %d / MaxHp : %d", _playerUnit.hp, _playerUnit.maxHp);
	DrawFormatString(0, 80, 0xffffff, "NowMp : %.2f / MaxMp : %.2f", _playerUnit.mp, _playerUnit.maxMp);

	int animNum = MV1GetAnimNum(_playerUnit.modelH);
	DrawFormatString(0, 200, 0xffffff, "AnimNum : %d", animNum);

	for (int i = 0; i < animNum; i++)
		DrawFormatString(0, 220 + (i * 20), 0xffffff, "[%d], %s", i, MV1GetAnimName(_playerUnit.modelH, i));

#endif
}

void Player::SetHit(int damage)
{
	if (!_playerUnit.isHit)
	{
		_playerUnit.isHit = true;
		_damagedCount = 0;

		_playerUnit.hp -= damage;
	}
}

void Player::JustDodgeEffect()
{
	_playerUnit.ultCharge += ULT_CHARGE_AMOUNT;
	if (_playerUnit.ultCharge >= _playerUnit.maxUltCharge)
		_playerUnit.ultCharge = _playerUnit.maxUltCharge;

	_playerUnit.hp += HP_HEAL_AMOUNT;
	if (_playerUnit.hp >= _playerUnit.maxHp)
		_playerUnit.hp = _playerUnit.maxHp;

	_playerUnit.mp += MP_GAIN_AMOUNT;
	if (_playerUnit.mp >= _playerUnit.maxMp)
		_playerUnit.mp = _playerUnit.maxMp;

	p_Dodge->ResetDodgeCoolCount();
}

void Player::UpdateState(std::shared_ptr<Input> pInput)
{
	int lx = pInput->GetLeftStickX();
	int ly = pInput->GetLeftStickY();

	if (_playerUnit.hp <= 0)
	{
		_playerUnit.nowState = CharacterState::Dead;
		return;
	}

	AnimInfo nowAnim = TranslateState(_playerUnit.nowState);

	if (nowAnim.isLoop == false)
	{
		if (!p_AManager->IsFinished()) return;
	}

	if (_playerUnit.isHit)
	{
		_playerUnit.nowState = CharacterState::HitStun;
		return;
	}

	if (_isDodge)
	{
		if (lx >= 0) _playerUnit.nowState = CharacterState::DodgeRight;
		else _playerUnit.nowState = CharacterState::DodgeLeft;

		return;
	}

	if (lx == 0 && ly == 0)
	{
		_playerUnit.nowState = CharacterState::Wait;
		return;
	}

	if (lx > 0 && std::abs(lx) > std::abs(ly))
	{
		_playerUnit.nowState = CharacterState::Approach;
		return;
	}

	if (lx < 0 && std::abs(lx) > std::abs(ly))
	{
		_playerUnit.nowState = CharacterState::MoveAway;
		return;
	}

	if (ly > 0 && std::abs(ly) > std::abs(lx))
	{
		_playerUnit.nowState = CharacterState::MoveRight;
		return;
	}

	if (ly < 0 && std::abs(ly) > std::abs(lx))
	{
		_playerUnit.nowState = CharacterState::MoveLeft;
		return;
	}
}