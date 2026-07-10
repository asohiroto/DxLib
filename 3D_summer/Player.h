#pragma once
#include "Character.h"
#include<DxLib.h>
#include<memory>

class Input;

class Player :
	public Character
{
public:
	Player();
	~Player() override;
	void Init(int Id);
	void Update(float cameraAngle, std::shared_ptr<Input> pInput, std::shared_ptr<Player> pOther);
	void Draw() override;
	// カメラの角度を取得する
	float GetAngle() const { return _angle; }

private:
	// Y軸方向の移動
	float _movementY;
	// 動く方向
	VECTOR _move;
	// カメラの回転を方向に変換した値
	VECTOR _movementDirection;
	// カメラの回転行列
	MATRIX _rotMatrix;
	// モデルの回転行列
	MATRIX _modelRotMatrix;
	// モデルが向く角度
	float _angle;
	// 回避中か
	bool _isDodge;
	// 現在の回避量
	float _dodgeMovement;
	// 回避秒数（回避距離算出に使用）
	int _dodgeCount;
	// 回避の方向
	VECTOR _dodgeDir;

	int _attackCount;

	bool _isAttacking = false;

	VECTOR _weakCollPos;

	VECTOR _posTemp;
};

