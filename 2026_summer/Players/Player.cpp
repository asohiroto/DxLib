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
	// 必殺技チャージ中の魔法陣の色（黄色）
	constexpr int ULT_CIRCLE_COLOR_R = 255;
	constexpr int ULT_CIRCLE_COLOR_G = 255;
	constexpr int ULT_CIRCLE_COLOR_B = 0;
	constexpr int ULT_CIRCLE_ALPHA = 255;
	// 通常時の魔法陣の拡大率
	constexpr float ULT_CIRCLE_NORMAL_SCALE = 1.0f;
	// マジックフューリーが出るときの魔法陣の拡大率
	constexpr float ULT_CIRCLE_FURY_SCALE = 1.5f;
	// ジャスト回避エフェクトの拡大率
	constexpr float DODGE_EFFECT_SCALE = 3.0f;
	// 通常時のジャスト回避判定の半径（存在しないため０）
	constexpr float NORM_JUST_RADIUS = 0.0f;
	// ジャスト回避判定の半径
	constexpr float JUST_DODGE_RADIUS = 300.0f;
	// ジャスト回避によって得ることのできる必殺技のチャージ量
	constexpr float ULT_CHARGE_AMOUNT = 20;
	// ジャスト回避によって得られるMPの量
	constexpr float MP_GAIN_AMOUNT = 50.0f;
	// ジャスト回避によって得られるHPの量
	constexpr int HP_HEAL_AMOUNT = 200;
	// 必殺技チャージの最大量
	constexpr float MAX_ULT_CHARGE = 100.0f;
	// 初期座標
	constexpr float INITIAL_POS_X = 1000.0f;
	constexpr float INITIAL_POS_Z = 1000.0f;
	// モデルの拡大率
	constexpr float MODEL_SCALE = 2.5f;
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
	_ultPressFrame(0),
	_damagedCount(0),
	_magicCircleH(-1),
	_circlePlayingH(-1),
	_ultCirclePlayingH(-1),
	_targetPlayingH(-1),
	_dodgeEffectPlayingH(-1),
	_isDodge(false), _playerMagics()
{
}

Player::~Player()
{
	// 再生中の各エフェクトを停止
	StopEffekseer3DEffect(_circlePlayingH);
	StopEffekseer3DEffect(_ultCirclePlayingH);
	StopEffekseer3DEffect(_dodgeEffectPlayingH);
}

void Player::Init(int handle, EffectHandles playerMagics, SeHandles se)
{
	// プレイヤーのキャラクターデータの初期設定
	_playerUnit.modelH = handle;
	_playerUnit.pos = VGet(INITIAL_POS_X, 0.0f, INITIAL_POS_Z);
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

	_playerMagics = playerMagics;

	// モデルの拡大
	MV1SetScale(_playerUnit.modelH, VGet(MODEL_SCALE, MODEL_SCALE, MODEL_SCALE));

	// 各ポインタの初期化
	p_Move = std::make_shared<PlayerMove>();
	p_Move->Init();
	p_Dodge = std::make_shared<PlayerDodge>();
	p_Dodge->Init();
	p_Shot = std::make_shared<MagicShot>();
	p_Shot->Init();
	p_Shot->SetMagicShotH(_playerMagics.shotHandle);
	p_Missile = std::make_shared<MagicMissile>();
	p_Missile->Init();
	p_Missile->SetMagicMissileH(_playerMagics.missileHandle);
	p_Fury = std::make_shared<MagicFury>();
	p_Fury->Init();
	p_Fury->SetMagicFuryH(_playerMagics.furyHandle);
	p_Beam = std::make_shared<MagicBeam>();
	p_Beam->Init();
	p_Beam->SetMagicBeamH(_playerMagics.beamHandle);
	p_AManager = std::make_shared<AnimManager>();
	p_AManager->Init(_playerUnit.modelH);

	_magicCircleH = _playerMagics.circleHandle;

	p_AManager->AnimChange(TranslateState(_playerUnit.nowState));

	_gameSE = se;
}

void Player::End()
{

}

void Player::Update(const std::shared_ptr<Input>& pInput, const std::shared_ptr<Camera>& pCamera, const std::shared_ptr<MagicManager>& pManager)
{
	// 各種経過フレームカウンタを進める
	_pressFrame++;
	_ultPressFrame++;
	_damagedCount++;

	// 魔力を最大値まで自動回復
	_playerUnit.mp += AUTO_MP_HEAL;
	if (_playerUnit.mp >= _playerUnit.maxMp)
		_playerUnit.mp = _playerUnit.maxMp;

	// 右スティックの入力を取得
	int rx = pInput->GetRightStickX();

	// 回避中ならジャスト回避判定の半径を有効化する
	if (p_Dodge->IsDodge())
		_playerUnit.justRadius = JUST_DODGE_RADIUS;
	else
		_playerUnit.justRadius = NORM_JUST_RADIUS;

	// 埋まり防止用
	if (_playerUnit.pos.y <= 0.0f) _playerUnit.pos.y = 0.0f;

	// 被弾後は一定フレームだけ色を変え、経過後に元の色と非被弾状態に戻す
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
	bool wasDodge = _isDodge;
	_isDodge = p_Dodge->IsDodge();

	// 回避が終了した瞬間にジャスト回避エフェクトを止める
	if (wasDodge && !_isDodge)
		StopEffekseer3DEffect(_dodgeEffectPlayingH);

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

	// ロックオン中は向きと正面ベクトルを敵方向に固定する
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

	// Bを推している時間を計測
	if (pInput->IsTrigger(PAD_INPUT_3))
	{
		_pressFrame = 0;

		// 前の魔法陣が残っていれば先に止める
		if (_circlePlayingH != -1) StopEffekseer3DEffect(_circlePlayingH);

		_circlePlayingH = PlayEffekseer3DEffect(_magicCircleH);
		SetPosPlayingEffekseer3DEffect(_circlePlayingH, circlePos.x, circlePos.y + CIRCLE_HEIGHT_OFFSET, circlePos.z);

		PlaySoundMem(_gameSE.circleH, DX_PLAYTYPE_LOOP);
	}

	// マジックミサイル生成になればカメラがロックオン
	if (pInput->IsPress(PAD_INPUT_3))
	{
		SetPosPlayingEffekseer3DEffect(_circlePlayingH, circlePos.x, circlePos.y + CIRCLE_HEIGHT_OFFSET, circlePos.z);

		SetRotationPlayingEffekseer3DEffect(_circlePlayingH, 0.0f, facing, 0.0f);

		if (_pressFrame < SHOT_SWITCH) pCamera->SetCameraMode(false);
		else pCamera->SetCameraMode(true);
	}

	// 離したときの時間で生成する魔法を切り替え
	if (pInput->IsRelease(PAD_INPUT_3))
	{
		StopSoundMem(_gameSE.circleH);
		if (_pressFrame < SHOT_SWITCH)
		{
			float remainMp = _playerUnit.mp - p_Shot->GetUseMp();
			if (remainMp >= 0)
			{
				_playerUnit.mp = remainMp;
				_frontVec = GetCameraFrontVector();

				// 画面中心からのレイキャストが敵に当たっていれば、その敵に向かって飛ばす
				if (pCamera->IsRayHitEnemy())
				{
					_frontVec = VNorm(VSub(pCamera->GetRayHitEnemyPos(), shotPos));
				}

				PlaySoundMem(_gameSE.shotH, DX_PLAYTYPE_BACK);
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
				PlaySoundMem(_gameSE.missileH, DX_PLAYTYPE_BACK);
				p_Missile->GenerateMissile(shotPos, _frontVec, false, pManager);
				_playerUnit.nowState = CharacterState::Missile;
				p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
			}
		}
		StopEffekseer3DEffect(_circlePlayingH);

		pCamera->SetCameraMode(false);
	}

	// Aの入力時間を計測し、必殺技を発動する（チャージが半分未満の場合は何も反応しない）
	if (_playerUnit.ultCharge >= _playerUnit.maxUltCharge / 2)
	{
		// Aを押した瞬間からロックオンカメラにし、黄色い魔法陣を出す
		if (pInput->IsTrigger(PAD_INPUT_4))
		{
			_ultPressFrame = 0;
			pCamera->SetCameraMode(true);

			// 元の魔法陣が残っていれば消す
			StopSoundMem(_gameSE.circleH);
			StopEffekseer3DEffect(_circlePlayingH);

			// 前の魔法陣が残っていれば先に止める
			if (_ultCirclePlayingH != -1) StopEffekseer3DEffect(_ultCirclePlayingH);

			_ultCirclePlayingH = PlayEffekseer3DEffect(_magicCircleH);
			SetPosPlayingEffekseer3DEffect(_ultCirclePlayingH, circlePos.x, circlePos.y + CIRCLE_HEIGHT_OFFSET, circlePos.z);
			SetColorPlayingEffekseer3DEffect(_ultCirclePlayingH, ULT_CIRCLE_COLOR_R, ULT_CIRCLE_COLOR_G, ULT_CIRCLE_COLOR_B, ULT_CIRCLE_ALPHA);
		}

		// 押している間は魔法陣を追従させる
		if (pInput->IsPress(PAD_INPUT_4))
		{
			SetPosPlayingEffekseer3DEffect(_ultCirclePlayingH, circlePos.x, circlePos.y + CIRCLE_HEIGHT_OFFSET, circlePos.z);
			SetRotationPlayingEffekseer3DEffect(_ultCirclePlayingH, 0.0f, facing, 0.0f);

			// 放つ技がマジックフューリーになる条件を満たしていれば魔法陣を拡大する
			if (_ultPressFrame >= SHOT_SWITCH && _playerUnit.ultCharge >= _playerUnit.maxUltCharge)
				SetScalePlayingEffekseer3DEffect(_ultCirclePlayingH, ULT_CIRCLE_FURY_SCALE, ULT_CIRCLE_FURY_SCALE, ULT_CIRCLE_FURY_SCALE);
			else
				SetScalePlayingEffekseer3DEffect(_ultCirclePlayingH, ULT_CIRCLE_NORMAL_SCALE, ULT_CIRCLE_NORMAL_SCALE, ULT_CIRCLE_NORMAL_SCALE);
		}

		// 離したときの押していた時間で発動する必殺技を切り替え
		if (pInput->IsRelease(PAD_INPUT_4))
		{
			if (_ultPressFrame >= SHOT_SWITCH && _playerUnit.ultCharge >= _playerUnit.maxUltCharge)
			{
				p_Fury->GenerateFury(pManager->GetEnePos(), VGet(0.0f, -1.0f, 0.0f), false, pManager);
				_playerUnit.nowState = CharacterState::Fury;
				PlaySoundMem(_gameSE.furyH, DX_PLAYTYPE_BACK);
				p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
				_playerUnit.ultCharge = 0;
			}
			else
			{
				p_Beam->GenerateBeam(shotPos, _frontVec, false, pManager);
				_playerUnit.nowState = CharacterState::Beam;
				PlaySoundMem(_gameSE.beamH, DX_PLAYTYPE_BACK);
				p_AManager->AnimChange(TranslateState(_playerUnit.nowState));
				_playerUnit.ultCharge -= (_playerUnit.maxUltCharge / 2);
			}

			StopEffekseer3DEffect(_ultCirclePlayingH);
			pCamera->SetCameraMode(false);
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
	// プレイヤーモデルを描画
	MV1DrawModel(_playerUnit.modelH);

	p_Dodge->Draw();
	p_AManager->Draw();

#ifdef _DEBUG
	DrawHitBox(_playerUnit);
	// ショットが存在していれば弾も描画
	if (p_Shot->IsExist()) p_Shot->Draw();

	// HP/MPの数値をデバッグ表示
	DrawFormatString(0, 60, 0xffffff, "NowHp : %d / MaxHp : %d", _playerUnit.hp, _playerUnit.maxHp);
	DrawFormatString(0, 80, 0xffffff, "NowMp : %.2f / MaxMp : %.2f", _playerUnit.mp, _playerUnit.maxMp);

	// モデルが持つ全アニメーション名を一覧表示
	int animNum = MV1GetAnimNum(_playerUnit.modelH);
	DrawFormatString(0, 200, 0xffffff, "AnimNum : %d", animNum);

	for (int i = 0; i < animNum; i++)
		DrawFormatString(0, 220 + (i * 20), 0xffffff, "[%d], %s", i, MV1GetAnimName(_playerUnit.modelH, i));

#endif
}

void Player::SetHit(int damage)
{
	// 既に被弾処理中でなければダメージを適用する
	if (!_playerUnit.isHit)
	{
		_playerUnit.isHit = true;
		_damagedCount = 0;

		_playerUnit.hp -= damage;

		// 攻撃を受けたら出ている魔法陣を消す
		StopSoundMem(_gameSE.circleH);
		StopEffekseer3DEffect(_circlePlayingH);
		StopEffekseer3DEffect(_ultCirclePlayingH);
	}
}

void Player::JustDodgeEffect()
{
	// 閾値をまたいだかどうかを判定するため変化前のチャージ量を保存
	int ultTemp = _playerUnit.ultCharge;
	_playerUnit.ultCharge += ULT_CHARGE_AMOUNT;
	// チャージが最大に達したら専用SEを再生し、上限でクランプ
	if (_playerUnit.ultCharge >= _playerUnit.maxUltCharge)
	{
		PlaySoundMem(_gameSE.secondChargeH, DX_PLAYTYPE_BACK);
		_playerUnit.ultCharge = _playerUnit.maxUltCharge;
	}
	// チャージが半分に達した瞬間なら専用SEを再生
	else if (ultTemp < _playerUnit.maxUltCharge / 2 && _playerUnit.ultCharge >= _playerUnit.maxUltCharge / 2)
	{
		PlaySoundMem(_gameSE.firstChargeH, DX_PLAYTYPE_BACK);
	}

	// HPを回復（上限あり）
	_playerUnit.hp += HP_HEAL_AMOUNT;
	if (_playerUnit.hp >= _playerUnit.maxHp)
		_playerUnit.hp = _playerUnit.maxHp;

	// MPを回復（上限あり）
	_playerUnit.mp += MP_GAIN_AMOUNT;
	if (_playerUnit.mp >= _playerUnit.maxMp)
		_playerUnit.mp = _playerUnit.maxMp;

	// ジャスト回避エフェクトを再生
	_dodgeEffectPlayingH = PlayEffekseer3DEffect(_playerMagics.dodgeHandle);
	SetPosPlayingEffekseer3DEffect(_dodgeEffectPlayingH, _playerUnit.pos.x, _playerUnit.pos.y, _playerUnit.pos.z);
	SetScalePlayingEffekseer3DEffect(_dodgeEffectPlayingH, DODGE_EFFECT_SCALE, DODGE_EFFECT_SCALE, DODGE_EFFECT_SCALE);

	// 回避成功SEを再生し、クールダウンをリセット
	PlaySoundMem(_gameSE.dodgeH, DX_PLAYTYPE_BACK);
	p_Dodge->ResetDodgeCoolCount();
}

void Player::SetUltCharge(int amount)
{
	// 閾値をまたいだかどうかを判定するため変化前のチャージ量を保存
	int ultTemp = _playerUnit.ultCharge;
	_playerUnit.ultCharge += amount;
	// チャージが最大に達したら専用SEを再生し、上限でクランプ
	if (_playerUnit.ultCharge >= _playerUnit.maxUltCharge)
	{
		PlaySoundMem(_gameSE.secondChargeH, DX_PLAYTYPE_BACK);
		_playerUnit.ultCharge = _playerUnit.maxUltCharge;
	}
	// チャージが半分に達した瞬間なら専用SEを再生
	else if (ultTemp < _playerUnit.maxUltCharge / 2 && _playerUnit.ultCharge >= _playerUnit.maxUltCharge / 2)
	{
		PlaySoundMem(_gameSE.firstChargeH, DX_PLAYTYPE_BACK);
	}
}

void Player::UpdateState(const std::shared_ptr<Input>& pInput)
{
	// 左スティックの入力を取得
	int lx = pInput->GetLeftStickX();
	int ly = pInput->GetLeftStickY();

	// HPが0以下なら死亡ステートに固定する
	if (_playerUnit.hp <= 0)
	{
		_playerUnit.nowState = CharacterState::Dead;
		return;
	}

	AnimInfo nowAnim = TranslateState(_playerUnit.nowState);

	// ループしないアニメーション（攻撃等）は再生完了までステートを変えない
	if (nowAnim.isLoop == false)
	{
		if (!p_AManager->IsFinished()) return;
	}

	// 被弾中なら硬直ステートにする
	if (_playerUnit.isHit)
	{
		_playerUnit.nowState = CharacterState::HitStun;
		return;
	}

	// 回避中は入力方向に応じて左右いずれかの回避ステートにする
	if (_isDodge)
	{
		if (lx >= 0) _playerUnit.nowState = CharacterState::DodgeRight;
		else _playerUnit.nowState = CharacterState::DodgeLeft;

		return;
	}

	// 入力が無ければ待機ステートにする
	if (lx == 0 && ly == 0)
	{
		_playerUnit.nowState = CharacterState::Wait;
		return;
	}

	// 縦横のうち入力の大きい方の軸を採用して移動ステートを決定する
	if (ly > 0 && std::abs(ly) > std::abs(lx))
	{
		_playerUnit.nowState = CharacterState::MoveAway;
		return;
	}

	if (ly < 0 && std::abs(ly) > std::abs(lx))
	{
		_playerUnit.nowState = CharacterState::Approach;
		return;
	}

	if (lx > 0 && std::abs(lx) > std::abs(ly))
	{
		_playerUnit.nowState = CharacterState::MoveRight;
		return;
	}

	if (lx < 0 && std::abs(lx) > std::abs(ly))
	{
		_playerUnit.nowState = CharacterState::MoveLeft;
		return;
	}
}