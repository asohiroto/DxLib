#pragma once
#include <DxLib.h>
#include <memory>

class Input;

class PlayerMove
{
public:
	PlayerMove();
	virtual ~PlayerMove();
	void Init();
	void End();
	void Update(const std::shared_ptr<Input>& pInput, float cameraAngle);
	void Draw();
	// 移動量のゲッター
	VECTOR GetMovement() const { return _movementDirection; }
private:
	// 移動量
	VECTOR _move;
	// 回転行列
	MATRIX _rotMatrix;
	// 移動方向
	VECTOR _movementDirection;
	// 入力値の保存
	int _mx, _mz;
};