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
	void Update(float cameraAngle);
	void Draw() override;

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
	// インプットクラスの共有ポインタ
	std::shared_ptr<Input> p_Input;
};

