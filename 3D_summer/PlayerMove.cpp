#include "PlayerMove.h"
#include"GameDefine.h"
#include<DxLib.h>
#include<algorithm>

using namespace GameDefine;

//void PlayerMove::WeakAttack()
//{
//
//}
//
//void PlayerMove::StrongAttack()
//{
//}
//
//void PlayerMove::Dodge()
//{
//}

void PlayerMove::Move(VECTOR& pos, VECTOR& move,  int inputX, int inputZ, float cameraAngle, int& modelH)
{
	pos = VGet(0.0f, 0.0f, 0.0f);

	// 移動量に代入
	move = VGet(inputX, 0.0f, -inputZ);

	// スティックの傾き具合を計算
	float len = VSize(move);
	// 0.0～1.0の割合に変換
	float rate = len / 1000;

	// 正規化
	move = VNorm(move);

	// 移動速度（最大で10.0f）
	float speed = MOVE_SPEED * rate;

	// 移動速度を反映した移動量を決定
	move = VScale(move, speed);

	// カメラの回転角から行列を作成し、移動方向をカメラ基準に変換
	MATRIX rotMatrix = MGetRotY(cameraAngle);
	VECTOR movementDirection = VTransform(move, rotMatrix);

	float angle = 0.0f;

	// モデルが向く方向を定める
	if (VSize(movementDirection) > 0.0f)
	{
		angle = atan2f(movementDirection.x, movementDirection.z) + DX_PI_F;
	}

	// angleのほうを向かせる
	MV1SetRotationXYZ(modelH, VGet(0.0f, angle, 0.0f));

	// 位置を更新
	pos = VAdd(pos, movementDirection);

	// 移動制限
	pos.x = std::clamp(static_cast<int>(pos.x), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));
	pos.z = std::clamp(static_cast<int>(pos.z), -static_cast<int>(GRID_SIZE * GRID_NUM / 2), static_cast<int>(GRID_SIZE * GRID_NUM / 2));
}