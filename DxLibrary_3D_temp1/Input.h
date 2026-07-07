#pragma once

class Input
{
public:
	Input();
	virtual ~Input();
	void Init();
	void Update();
	// 押した瞬間を取得
	bool IsTrigger(int key);
	// 押されているかを取得
	bool IsPress(int key);
	// 離した瞬間を取得
	bool IsRelease(int key);
	// 右スティックの入力値を取得
	int GetRightStickX() const { return _rx; }
	int GetRightStickY() const { return _ry; }

private:
	// 前フレームの入力状態
	int _lastPad;
	// 現在フレームの入力状態
	int _nowPad;
	// 右スティックのx方向の入力状態
	int _rx;
	// 右スティックのy方向入力状態
	int _ry;
};