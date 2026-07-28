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
	void Update(std::shared_ptr<Input> pInput);
	void Draw();
	// 移動量のゲッター
	VECTOR GetMove() const { return _move; }
private:
	// 移動量
	VECTOR _move;
};