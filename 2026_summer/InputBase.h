#pragma once
#include "Action.h"

class InputBase
{
public:
	InputBase() {};
	virtual ~InputBase() {};
	virtual void Init() {};
	virtual void Update() {};
	// 押しているかを判定
	virtual bool IsPress() {};
	// 押された瞬間を判定
	virtual bool IsTrigger() {};
	// 離した瞬間を判定
	virtual bool IsRelease() {};
	// 起こしたいアクションに応じて対応するキーコードを返す
	virtual int GetActionKey(Action action) {};

protected:
	// １フレーム前の入力状態
	int _lastKey = 0;
	// 現在フレームの入力状態
	int _nowKey = 0;
};