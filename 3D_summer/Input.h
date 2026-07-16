#pragma once
#include"GameDefine.h"
#include<DxLib.h>
#include<cmath>

using namespace GameDefine;

class Input
{
public:
	Input();
	virtual ~Input();
	void Init(int padNum);
	void Update();
	// 押した瞬間を取得
	bool IsTrigger(int key);
	// 押されているかを取得
	bool IsPress(int key);
	// 離した瞬間を取得
	bool IsRelease(int key);
	// 右スティックのX方向入力値を取得
	int GetRightStickX() const { return _rx; }
	// 右スティックのY方向入力値を取得
	int GetRightStickY() const { return _ry; }
	// 左スティックのX方向入力値を取得
	int GetLeftStickX() const { return _lx; }
	// 左スティックのY方向入力値を取得
	int GetLeftStickY() const { return _ly; }
	// Lスティックが倒されているか（入力されているか）を返す関数
	bool IsTiltingL() const { return (std::abs(_lx) > STICK_DEAD_ZONE || std::abs(_ly) > STICK_DEAD_ZONE); }
	// Rスティックが倒されているか（入力されているか）を返す関数
	bool IsTiltingR() const { return (std::abs(_rx) > STICK_DEAD_ZONE || std::abs(_ry) > STICK_DEAD_ZONE); }

private:
	// 前フレームの入力状態
	int _lastPad;
	// 現在フレームの入力状態
	int _nowPad;
	// 右スティックのx方向の入力状態
	int _rx;
	// 右スティックのy方向の入力状態
	int _ry;
	// 左スティックのx方向の入力状態
	int _lx;
	// 左スティックのy方向の入力状態
	int _ly;
	// 現在使用中のパッド
	int _usePadNum;
};