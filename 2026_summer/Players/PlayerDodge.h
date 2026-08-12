#pragma once
#include <DxLib.h>
#include <memory>

class Input;

class PlayerDodge
{
public:
	PlayerDodge();
	virtual ~PlayerDodge();
	void Init();
	void End();
	void Update(std::shared_ptr<Input> pInput, float cameraAngle);
	void Draw();
	// 回避方向の計算
	void CalDirection(std::shared_ptr<Input> pInput, float cameraAngle);
	// 回避位置のゲッター
	VECTOR GetDodgePos() const { return _dodgedPos; }

private:
	// 回避方向
	VECTOR _dodgeDir;
	// 回避を行うか
	bool _isDodge;
	// 実際の回避距離
	float _dodgeDistance;
	// 回避先座標
	VECTOR _dodgedPos;
};