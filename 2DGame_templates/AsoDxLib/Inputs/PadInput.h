#pragma once
#include <cmath>

class PadInput
{
public:
	PadInput();
	virtual ~PadInput();
	void Init();
	void Update();
	void DebugDraw() const;

	// 押しているかを判定
	bool IsPress(int key) const;
	// 押された瞬間を判定
	bool IsTrigger(int key) const;
	// 離した瞬間を判定
	bool IsRelease(int key) const;
	// 各スティックのゲッター
	int GetRightStickX() const { return _rx; }
	int GetRightStickY() const { return _ry; }
	int GetLeftStickX() const { return _lx; }
	int GetLeftStickY() const { return _ly; }
	// 右スティックに入力があるか
	bool IsTiltingR() const { return (std::abs(_rx) > 0 || std::abs(_ry) > 0); }
	// 左スティックに入力があるか
	bool IsTiltingL() const { return (std::abs(_lx) > 0 || std::abs(_ly) > 0); }

protected:
	// １フレーム前の入力状態
	int _lastKey;
	// 現在フレームの入力状態
	int _nowKey;
	// 右スティックの入力状態
	int _rx, _ry;
	// 左スティックの入力状態
	int _lx, _ly;
};