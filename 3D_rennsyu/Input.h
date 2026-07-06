#pragma once

class Input final
{
private:
	// シングルトン化の準備
	Input() = default;
	~Input() = default;
	// コピー禁止
	Input(const Input&) = delete;
	Input& operator = (const Input&) = delete;
	// ムーブ禁止
	Input(Input&&) = delete;
	Input& operator = (Input&&) = delete;
private:
	int _lastPad = 0;
	int _nowPad = 0;

	int _rx = 0;
	int _ry = 0;

public:
	static Input& GetInstance()
	{
		static Input instance;
		return instance;
	}

	void Update();
	bool IsPress(int key);
	bool IsTrigger(int key);
	bool IsRelease(int key);

	int GetRightStickX() const { return _rx; }
	int GetRightStickY() const { return _ry; }

};