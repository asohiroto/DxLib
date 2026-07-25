#include"Player.h"
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
	_isDodge(false),
	_dodgeMovement(0),
	_dodgeCount(0),
	_dodgeDir(),
	_attackCount(0),
	_isWeakAttacking(false),
	_weakCollPos(),
	_attackDirection(),
	_idTemp(0),
	_isDamaged(false),
	_wDamagedCount(0),
	_sDamagedCount(0),
	_playerColor(0xff0000),
	_strongCollPos(),
	_isStrongAttacking(false),
	_attachAnimIndex(-1),
	_animCount(0),
	_totalTime(0),
	_blendRate(0),
	_isAnimChange(false),
	_nextAttachAnimIndex(-1),
	_prevAttachAnimIndex(-1),
	_state(PlayerState::Idle),
	_isDash(false),
	_playerSpeed(MOVE_SPEED),
	_animSpeed(0.5f),
	_hitstopRequestFrame(0),
	_playerDodgeCoolCount(0),
	_attackWindUpCount(0),
	_isWDamageCooldown(false),
	_isSDamageCooldown(false)
{

}

Player::~Player()
{
	MV1DeleteModel(_modelH);
}

void Player::Init(int id)
{
	// プレイヤーの体力を最大値に初期化
	_playerHp = _maxPlayerHp;

	// プレイヤーに応じてモデルを変化
	if (id == 1)
	{
		_modelH = MV1LoadModel("data/model_army.mv1");
		_pos = VGet(500.0f, 0.0f, 200.0f);
		_idTemp = id;
	}
	else if (id == 2)
	{
		_modelH = MV1LoadModel("data/model_army.mv1");
		_pos = VGet(-500.0f, 0.0f, -200.0f);
		_idTemp = id;
	}

	//アニメーション関係初期設定
	_attachAnimIndex = MV1AttachAnim(_modelH, 0, -1, false);
	_totalTime = MV1GetAttachAnimTotalTime(_modelH, _attachAnimIndex);

	// モデルを拡大
	MV1SetScale(_modelH, VGet(3.0f, 3.0f, 3.0f));

	// 安全策
	assert(_modelH != -1);

	// モデルの回転、移動を固定
	int rootFrame = 0;
	MATRIX baseMat = MV1GetFrameBaseLocalMatrix(_modelH, rootFrame);
	MV1SetFrameUserLocalMatrix(_modelH, rootFrame, baseMat);

	// プレイヤーのコリジョン情報を設定
	SetCollision(true, CollisionType::Capsule, PLAYER_COL_RADIUS, -1);
}

void Player::Update(float cameraAngle, std::shared_ptr<Input> pInput, std::shared_ptr<Player> pOther)
{
	bool isAttackState = ((_state == PlayerState::WAttack) || (_state == PlayerState::SAttack));

	_isWDamageCooldown = _wDamagedCount > WEAK_DAMAGED_COOLDOWN;
	_isSDamageCooldown = _sDamagedCount > STRONG_DAMAGED_COOLDOWN;

	// カウントの更新
	_wDamagedCount++;
	_sDamagedCount++;
	_dodgeCount++;
	_attackCount++;
	_attackWindUpCount++;
	_playerDodgeCoolCount++;
	_animCount += _animSpeed;

	switch (_state)
	{
	case PlayerState::Idle:
		UpdateIdle(pInput);
		break;
	case PlayerState::Move:
		UpdateMove(pInput, pOther, cameraAngle);
		break;
	case PlayerState::WAttack:
		UpdateWAttack(pInput);
		break;
	case PlayerState::SAttack:
		UpdateSAttack(pInput);
		break;
	case PlayerState::Dodge:
		UpdateDodge(pInput);
		break;
	}

	if (!isAttackState && !_isDodge)
	{
		if (pInput->IsTiltingL())
		{
			if (pInput->IsTrigger(PAD_INPUT_A) && _playerDodgeCoolCount > DODGE_COOLDOWN_FRAME)
			{
				ChangeState(PlayerState::Dodge);
			}
			else
			{
				ChangeState(PlayerState::Move);
			}
		}
		else
		{
			ChangeState(PlayerState::Idle);
		}

		if (pInput->IsTrigger(PAD_INPUT_X))
		{
			ChangeState(PlayerState::WAttack);
		}
		else if (pInput->IsTrigger(PAD_INPUT_C))
		{
			ChangeState(PlayerState::SAttack);
		}
	}



	// Lスティックの入力がある間、攻撃判定の回転方向を、プレイヤーの向いている正規化ベクトルで更新
	if (pInput->IsTiltingL() && !isAttackState) _attackDirection = VNorm(_movementDirection);

	// 回避の更新処理
	if (_isDodge)
	{
		// 方向に速度をかけて、_posを更新
		_pos = VAdd(_pos, VScale(_dodgeDir, DODGE_SPEED));
		// 回避距離
		_dodgeMovement = DODGE_SPEED * _dodgeCount;

		// 実際の回避距離が設定値を超えたら回避終了
		if (_dodgeMovement >= DODGE_DISTANCE)
		{
			_isDodge = false;
			_playerDodgeCoolCount = 0;

			// 入力がある場合はmoveに、なければidleに遷移
			if (pInput->IsTiltingL())
			{
				ChangeState(PlayerState::Move);
			}
			else
			{
				ChangeState(PlayerState::Idle);
			}
		}
	}

	// 弱攻撃を行ってから、特定フレーム経過すると弱攻撃を終了する
	if (_state == PlayerState::WAttack && _attackCount >= WEAK_ATTACKING_FRAME)
	{
		_isWeakAttacking = false;
		if (pInput->IsTiltingL())
		{
			ChangeState(PlayerState::Move);
			_attackWindUpCount = 0;
		}
		else
		{
			ChangeState(PlayerState::Idle);
			_attackWindUpCount = 0;
		}
	}

	// 強攻撃を行ってから、特定フレーム経過すると強攻撃を終了する
	if (_state == PlayerState::SAttack && _attackCount >= STRONG_ATTACKING_FRAME)
	{
		_isStrongAttacking = false;
		if (pInput->IsTiltingL())
		{
			ChangeState(PlayerState::Move);
			_attackWindUpCount = 0;
		}
		else
		{
			ChangeState(PlayerState::Idle);
			_attackWindUpCount = 0;
		}
	}

	// モデルの回転処理-----------------------------------------------------------

	// 攻撃中でなければ、モデルが向く方向を定める
	if (VSize(_movementDirection) > 0.0f && !isAttackState)
	{
		_angle = atan2f(_movementDirection.x, _movementDirection.z) + DX_PI_F;
	}

	// angleのほうを向かせる
	MV1SetRotationXYZ(_modelH, VGet(0.0f, _angle, 0.0f));

	// ---------------------------------------------------------------------------

	// アニメーションの再生処理---------------------------------------------------

	if (_animCount >= _totalTime)
	{
		_animCount = 0.0f;
	}

	MV1SetAttachAnimTime(_modelH, _attachAnimIndex, _animCount);

	if (_isAnimChange)
	{
		_blendRate += 0.1f;

		if (_blendRate >= 1.0f)
		{
			_blendRate = 1.0f;

			if (_prevAttachAnimIndex != -1)
			{
				MV1DetachAnim(_modelH, _prevAttachAnimIndex);
				_prevAttachAnimIndex = -1;
			}
		}

		MV1SetAttachAnimBlendRate(_modelH, _prevAttachAnimIndex, 1.0f - _blendRate);
		MV1SetAttachAnimBlendRate(_modelH, _attachAnimIndex, _blendRate);

		if (_blendRate >= 1.0f)
		{
			_isAnimChange = false;
		}
	}

	// --------------------------------------------------------------------------

	// 各種更新処理 -------------------------------------------------------------

	// 【デバッグ】ダメージクールダウンが終了すると色をもとに戻す
	if (_isSDamageCooldown || _isWDamageCooldown)
	{
		_playerColor = 0xff0000;
	}

	if (pInput->IsTrigger(PAD_INPUT_B))
	{
		if (!_isDash)
		{
			_playerSpeed = DASH_SPEED;
			_isDash = true;
		}
		else if (_isDash)
		{
			_playerSpeed = MOVE_SPEED;
			_isDash = false;
		}
	}


	// 移動制限
	_pos.x = std::clamp(static_cast<int>(_pos.x), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));
	_pos.z = std::clamp(static_cast<int>(_pos.z), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));

	// 当たり判定の更新
	CollProcess(pOther);
	// モデルを配置
	MV1SetPosition(_modelH, _pos);
}

void Player::Draw()
{
	// モデルの描画
	MV1DrawModel(_modelH);

#ifdef _DEBUG

	// 当たり判定用カプセルの表示
	DrawCapsule3D
	(
		VAdd(_pos, PLAYER_SEGMENT_FINISH_COR), VAdd(_pos, PLAYER_SEGMENT_START_COR),
		PLAYER_COL_RADIUS,
		16,
		_playerColor, _playerColor,
		false
	);

	// 【デバッグ】攻撃中は当たり判定のスフィアを描画
	if (_isWeakAttacking)
	{
		DrawSphere3D(_weakCollPos, WEAK_ATTACK_RADIUS, 24, 0xffffff, 0xffffff, false);
	}
	else if (_isStrongAttacking)
	{
		DrawSphere3D(_strongCollPos, STRONG_ATTACK_RADIUS, 24, 0xffffff, 0xffffff, false);
	}
#endif

}

void Player::AttackProcess(std::shared_ptr<Input> pInput, int type)
{
	if (type == 0)
	{
		// 弱攻撃の処理-----------------------------------------------------------------------------
		// 弱攻撃の当たり判定の中心座標の計算式
		VECTOR dir1 = VScale(_attackDirection, WEAK_ATTACK_POS_COR);
		VECTOR dir2 = VAdd(_pos, dir1);
		VECTOR dir3 = VAdd(dir2, WEAK_ATTACK_HEIGHT_COR);

		_weakCollPos = dir3;

		_isWeakAttacking = true;
	}
	else if (type == 1)
	{
		// 強攻撃の処理-----------------------------------------------------------------------------
		// 強攻撃の当たり判定の中心座標の計算式
		VECTOR dir1 = VScale(_attackDirection, STRONG_ATTACK_POS_COR);
		VECTOR dir2 = VAdd(_pos, dir1);
		VECTOR dir3 = VAdd(dir2, STRONG_ATTACK_HEIGHT_COR);

		_strongCollPos = dir3;

		_isStrongAttacking = true;
	}

	_attackCount = 0;
}

// アニメーション切り替え処理
void Player::AnimChange(int animIndex)
{
	// printfDx("%d\n", animIndex);

	if (_prevAttachAnimIndex != -1)
	{
		MV1DetachAnim(_modelH, _prevAttachAnimIndex);
	}

	_prevAttachAnimIndex = _attachAnimIndex;
	_attachAnimIndex = MV1AttachAnim(_modelH, animIndex, -1, false);
	_animCount = 0.0f;

	if (_isAnimChange)
	{
		_blendRate = 1.0f - _blendRate;
	}
	else
	{
		_blendRate = 0.0f;
	}

	_totalTime = MV1GetAttachAnimTotalTime(_modelH, _attachAnimIndex);
	_isAnimChange = true;

	switch (animIndex)
	{
	case 0:
		_animSpeed = 0.5f;
		break;
	case 1:
		_animSpeed = 0.90f;
		break;
	case 2:
		_animSpeed = 0.8f;
		break;
	case 3:
		_animSpeed = 0.5f;
		break;
	case 4:
		_animSpeed = 0.70f;
		break;
	}
}

void Player::ChangeState(PlayerState next)
{
	if (_state == next) return;

	_state = next;

	//printfDx("%d\n", next);

	// 関数の初期設定処理
	switch (next)
	{
	case PlayerState::Idle:
		_attackCount = 0;
		_attackWindUpCount = 0;
		AnimChange(0);
		break;
	case PlayerState::Move:
		_attackCount = 0;
		_attackWindUpCount = 0;
		AnimChange(3);
		break;
	case PlayerState::WAttack:
		_attackCount = 0;
		_attackWindUpCount = 0;
		AnimChange(1);
		break;
	case PlayerState::SAttack:
		_attackCount = 0;
		_attackWindUpCount = 0;
		AnimChange(4);
		break;
	case PlayerState::Dodge:
		_attackCount = 0;
		_attackWindUpCount = 0;
		// リセット
		_dodgeMovement = 0.0f;
		_dodgeCount = 0;
		AnimChange(2);
		break;
	}

}

// 待機状態の更新処理
void Player::UpdateIdle(std::shared_ptr<Input> pInput)
{

}

// 移動状態の更新処理
void Player::UpdateMove(std::shared_ptr<Input> pInput, std::shared_ptr<Player> pOther, float cameraAngle)
{
	// 移動量に代入
	_move = VGet(pInput->GetLeftStickX(), 0.0f, -pInput->GetLeftStickY());

	// スティックの傾き具合を計算
	float len = VSize(_move);

	// 0.0～1.0の割合に変換
	float rate = len / 1000;

	// 正規化
	_move = VNorm(_move);

	// 移動速度（最大で10.0f）
	float speed = _playerSpeed * rate;

	// 移動速度を反映した移動量を決定
	_move = VScale(_move, speed);

	// カメラの回転角から行列を作成し、移動方向をカメラ基準に変換
	_rotMatrix = MGetRotY(cameraAngle);

	// 移動量から向いている角度を出す
	_movementDirection = VTransform(_move, _rotMatrix);

	// 位置を更新
	//AnimChange(3);
	_pos = VAdd(_pos, _movementDirection);
}

// 弱攻撃状態の更新処理
void Player::UpdateWAttack(std::shared_ptr<Input> pInput)
{
	// 入力があって、攻撃中でないなら当たり判定を出す
	if (!IsAttacking() && _isWDamageCooldown)
	{
		if (_attackWindUpCount >= WEAK_ATTACK_ANIMATION_COR)
			AttackProcess(pInput, 0);
	}
}

// 強攻撃状態の更新
void Player::UpdateSAttack(std::shared_ptr<Input> pInput)
{
	// 攻撃中でないなら当たり判定を出す
	if (!IsAttacking() && _isSDamageCooldown)
	{
		if (_attackWindUpCount >= STRONG_ATTACK_ANIMATION_COR)
			AttackProcess(pInput, 1);
	}
}

// 回避状態の更新処理
void Player::UpdateDodge(std::shared_ptr<Input> pInput)
{
	// 回避先の設定
	VECTOR dodgePoint = VScale(_movementDirection, DODGE_DISTANCE);

	// 回避方向の設定
	_dodgeDir = VNorm(VSub(dodgePoint, _pos));
	_isDodge = true;
}

void Player::CollProcess(std::shared_ptr<Player> pOther)
{
	// プレイヤーとプレイヤーの当たり判定

	// 相手プレイヤーまでの距離を線分同士の距離の計算から算出
	float distToOther = Segment_Segment_MinLength(_pos,
		VAdd(_pos, VGet(0.0f, 450.0f, 0.0f)),
		pOther->GetPos(),
		VAdd(pOther->GetPos(), VGet(0.0f, 450.0f, 0.0f)));

	// 当たり判定のめりこみ量
	float diff = (_radius + pOther->GetRadius()) - distToOther;

	// 押し戻す方向
	VECTOR pullBackDir = VGet(_pos.x - pOther->GetPos().x, 0.0f, _pos.z - pOther->GetPos().z);

	// 当たり判定がめり込んでいるなら、押し戻しを行う
	if (diff >= 0)
	{
		_pos = VAdd(_pos, VScale(VNorm(pullBackDir), diff));
	}

	// プレイヤーと攻撃の当たり判定 -------------------------------------------------------------

	// 弱攻撃との当たり判定
	if (pOther->IsWeakAttacking() && !_isDodge)
	{
		// 当たり判定までの距離を線分と点の距離で計算
		float distToAttack = Segment_Point_MinLength
		(
			VAdd(_pos, PLAYER_SEGMENT_START_COR),
			VAdd(_pos, PLAYER_SEGMENT_FINISH_COR),
			pOther->GetWeakCollPos()
		);

		// 当たると、フラグを立てる
		if (_radius + WEAK_ATTACK_RADIUS >= distToAttack && _isWDamageCooldown)
		{
			_isDamaged = true;
		}

		// ダメージ判定の処理
		if (_isDamaged)
		{
			_playerColor = 0x00ffff;
			_isDamaged = false;
			_wDamagedCount = 0;

			// 相手の体力を減らす
			SetHp(GetHp() - WEAK_ATTACK_DAMAGE);
			if (GetHp() <= 0) SetHp(0);

			// 自身の体力を増やす
			pOther->SetHp(pOther->GetHp() + WEAK_ATTACK_ABSORB);
			if (pOther->GetHp() > (pOther->GetMaxHp() * 2))
			{
				pOther->SetHp(_maxPlayerHp * 2);
			}

			// ヒットストップフレームの設定
			_hitstopRequestFrame = WEAK_HITSTOP_FRAME;
		}
	}

	// 強攻撃との当たり判定
	if (pOther->IsStrongAttacking() && !_isDodge)
	{
		// 当たり判定までの距離を線分と点の距離で計算
		float distToAttack = Segment_Point_MinLength
		(
			VAdd(_pos, PLAYER_SEGMENT_START_COR),
			VAdd(_pos, PLAYER_SEGMENT_FINISH_COR),
			pOther->GetStrongCollPos()
		);

		// 当たると、フラグを立てる
		if (_radius + STRONG_ATTACK_RADIUS >= distToAttack && _isSDamageCooldown)
		{
			_isDamaged = true;
		}

		// ダメージ判定の処理
		if (_isDamaged)
		{
			_playerColor = 0x00ff00;
			_isDamaged = false;
			_sDamagedCount = 0;

			// 相手の体力を減らす
			SetHp(GetHp() - STRONG_ATTACK_DAMAGE);
			if (GetHp() <= 0) SetHp(0);

			// 自身の体力を増やす
			pOther->SetHp(pOther->GetHp() + STRONG_ATTACK_ABSORB);
			if (pOther->GetHp() > (pOther->GetMaxHp() * 2))
			{
				pOther->SetHp(_maxPlayerHp * 2);
			}

			// ヒットストップフレームの設定
			_hitstopRequestFrame = STRONG_HITSTOP_FRAME;
		}
	}
}

int Player::HitstopRequest()
{
	int frameTemp = _hitstopRequestFrame;
	_hitstopRequestFrame = 0;
	return frameTemp;
}