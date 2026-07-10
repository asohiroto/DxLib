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
	_modelRotMatrix(),
	_angle(0.0f),
	_isDodge(false),
	_dodgeDir()
{

}

Player::~Player()
{
	MV1DeleteModel(_modelH);
}

void Player::Init(int id)
{
	// プレイヤーに応じてモデルを変化
	if (id == 1)
	{
		_modelH = MV1LoadModel("data/gunjin.mv1");
		_pos = VGet(500.0f, 0.0f, 200.0f);
		idTemp = id;
	}
	else if (id == 2)
	{
		_modelH = MV1LoadModel("data/gunjin.mv1");
		_pos = VGet(-500.0f, 0.0f, -200.0f);
		idTemp = id;
	}

	MV1SetScale(_modelH, VGet(3.0f, 3.0f, 3.0f));

	// 安全策
	assert(_modelH != -1);

	// プレイヤーのコリジョン情報を設定
	SetCollision(true, CollisionType::Capsule, 120.0f, -1);
}

void Player::Update(float cameraAngle, std::shared_ptr<Input> pInput, std::shared_ptr<Player> pOther)
{
	// コントローラーの入力値の保存
	int analogX = 0;
	int analogZ = 0;
	analogX = pInput->GetLeftStickX();
	analogZ = pInput->GetLeftStickY();

	// 移動の処理-----------------------------------------------------------------

	// 移動量に代入
	_move = VGet(analogX, 0.0f, -analogZ);

	// スティックの傾き具合を計算
	float len = VSize(_move);
	// 0.0～1.0の割合に変換
	float rate = len / 1000;

	// 正規化
	_move = VNorm(_move);

	// 移動速度（最大で10.0f）
	float speed = MOVE_SPEED * rate;

	// 移動速度を反映した移動量を決定
	_move = VScale(_move, speed);

	// カメラの回転角から行列を作成し、移動方向をカメラ基準に変換
	_rotMatrix = MGetRotY(cameraAngle);
	if (idTemp == 1)
	{
		DrawFormatString(10, 300, 0xffffff, "%.2f", cameraAngle);
	}
	else
	{
		DrawFormatString(900, 300, 0xffffff, "%.2f", cameraAngle);
	}
	_movementDirection = VTransform(_move, _rotMatrix);

	// 位置を更新
	_pos = VAdd(_pos, _movementDirection);

	// 移動制限
	_pos.x = std::clamp(static_cast<int>(_pos.x), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));
	_pos.z = std::clamp(static_cast<int>(_pos.z), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));

	//----------------------------------------------------------------------------

	// 当たり判定の処理-----------------------------------------------------------

	float distToOther = Segment_Segment_MinLength
	(_pos, VAdd(_pos, VGet(0.0f, 450.0f, 0.0f)),
		pOther->GetPos(), VAdd(pOther->GetPos(), VGet(0.0f, 450.0f, 0.0f)));

	float diff = (_radius + pOther->GetRadius()) - distToOther;

	VECTOR pullBackDir = VGet(_pos.x - pOther->GetPos().x, 0.0f, _pos.z - pOther->GetPos().z);

	if (diff >= 0)
	{
		// 当たっているときの処理を書く
		_pos = VAdd(_pos, VScale(VNorm(pullBackDir), diff));
	}

	// ---------------------------------------------------------------------------

	// 回避の処理 ----------------------------------------------------------------

	// 回避中の秒数カウント
	_dodgeCount++;

	if (pInput->IsTrigger(PAD_INPUT_A) && !_isDodge)
	{
		// リセット
		_dodgeMovement = 0.0f;
		_dodgeCount = 0;
		// 回避先の設定
		VECTOR dodgePoint = VScale(_movementDirection, DODGE_DISTANCE);
		// 回避方向の設定
		_dodgeDir = VNorm(VSub(dodgePoint, _pos));
		_isDodge = true;
	}

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
		}
	}

	// ---------------------------------------------------------------------------

	// 攻撃関連の処理-------------------------------------------------------------

	_attackCount++;

	if (analogX > STICK_DEAD_ZONE || analogZ > STICK_DEAD_ZONE) _moveTemp = _movementDirection;

	VECTOR aso = VScale(_moveTemp, 20.0f);
	VECTOR aso2 = VAdd(_pos, aso);
	VECTOR aso3 = VAdd(aso2, VGet(0.0f, 400.0f, 0.0f));


	_weakCollPos = aso3;

	if (pInput->IsPress(PAD_INPUT_X) && !_isAttacking)
	{
		_attackCount = 0;

		_isAttacking = true;
	}

	if (_isAttacking && _attackCount >= 10)
	{
		_isAttacking = false;
	}


	//----------------------------------------------------------------------------

	// モデルの回転処理-----------------------------------------------------------

	// モデルが向く方向を定める
	if (VSize(_movementDirection) > 0.0f)
	{
		_angle = atan2f(_movementDirection.x, _movementDirection.z) + DX_PI_F;
	}

	// angleのほうを向かせる
	MV1SetRotationXYZ(_modelH, VGet(0.0f, _angle, 0.0f));

	// ---------------------------------------------------------------------------

	// モデルを配置
	MV1SetPosition(_modelH, _pos);
}

void Player::Draw()
{
	// モデルの描画
	MV1DrawModel(_modelH);
	// 当たり判定用カプセルの表示
	DrawCapsule3D(
		VAdd(_pos, VGet(0.0f, 450.0f, 0.0f)), VAdd(_pos, VGet(0.0f, 100.0f, 0.0f)),
		120.0f,
		16,
		0xff0000, 0xff0000,
		false
	);


	if (_isAttacking && _attackCount < 60)
	{
		DrawSphere3D(_weakCollPos, 100.0f, 24, 0xffffff, 0xffffff, false);
	}

	VECTOR aso = VScale(_moveTemp, 40.0f);
	DrawLine3D(_moveTemp, aso, GetColor(255, 255, 255));
}