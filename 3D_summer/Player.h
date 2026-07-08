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
	void Init() override;
	void Update(float cameraAngle, std::shared_ptr<Input> pInput) override;
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
};

